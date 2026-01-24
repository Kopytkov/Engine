#version 330 core
out vec4 FragColor;

// Константы рендеринга
const int   MAX_BOUNCES    = 4;      // Глубина рекурсии
const int   MAX_STEP_COUNT = 160;    // Макс. кол-во шагов маршинга (точность поиска поверхности)
const float MAX_TRACE_DIST = 150.0;  // Дальность прорисовки
const float HIT_THRESHOLD  = 0.001;  // Порог столкновения
const float SURFACE_BIAS   = 0.02;   // Смещение луча от поверхности (чтобы не было "дырок" и самопересечений)

// Параметры стола
const float TABLE_WIDTH  = 15.0;
const float TABLE_HEIGHT = 20.0;
const float TABLE_Z_POS  = -1.0; 

// Параметры бортов
const float BORDER_THICKNESS = 0.5; // Полу-толщина борта
const float BORDER_HEIGHT    = 1.5; // Полу-высота борта

// Uniforms (Камера, Свет, Материалы)
uniform vec3 cameraPos, cameraView, cameraUp, cameraRight;
uniform float tanFovHalf;
uniform vec2 resolution;

uniform vec3 pointLightPos, pointLightColor;
uniform float pointLightBrightness;
uniform vec3 globalLightDir, globalLightColor;
uniform float globalLightBrightness;

uniform sampler2D ballTextures[16];

struct BallMaterial {
    vec3 baseColor;
    float roughness;
    float metallic;
    float transmission;
    float refraction;
    int textureID;
};
uniform BallMaterial ballMaterials[16];

// Динамические параметры
uniform vec3 ballPositions[16];
uniform int ballCount;
uniform float ballRadius;

// SDF функции
float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p - center) - radius;
}

float sdBox(vec3 p, vec3 center, vec3 size) {
    vec3 q = abs(p - center) - size;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Сцена
float sceneSDF(vec3 p, out int hitType, out int ballIndex, bool ignoreTransparent) {
    hitType = 0; ballIndex = -1; 
    float min_dist = 1e10; // Изначально расстояние "бесконечное"

    // Стол
    float table = sdBox(p, vec3(0.0, 0.0, TABLE_Z_POS), vec3(TABLE_WIDTH, TABLE_HEIGHT, 1.0));
    if (table < min_dist) { 
        min_dist = table; 
        hitType = 1; 
    }

    // Борта
    float borders = 1e10;
    vec3 vBorderSideSize = vec3(BORDER_THICKNESS, TABLE_HEIGHT + BORDER_THICKNESS, BORDER_HEIGHT);
    borders = min(borders, sdBox(p, vec3(-TABLE_WIDTH, 0.0, 0.5), vBorderSideSize));
    borders = min(borders, sdBox(p, vec3( TABLE_WIDTH, 0.0, 0.5), vBorderSideSize));

    vec3 vBorderTopSize = vec3(TABLE_WIDTH + BORDER_THICKNESS, BORDER_THICKNESS, BORDER_HEIGHT);
    borders = min(borders, sdBox(p, vec3(0.0, -TABLE_HEIGHT, 0.5), vBorderTopSize));
    borders = min(borders, sdBox(p, vec3(0.0,  TABLE_HEIGHT, 0.5), vBorderTopSize));
    
    if (borders < min_dist) { 
        min_dist = borders; 
        hitType = 2; 
    }

    // Шары
    for (int i = 0; i < ballCount; ++i) {
        // Если это расчет тени, мы игнорируем прозрачные шары, чтобы свет проходил сквозь них
        if (ignoreTransparent && ballMaterials[i].transmission > 0.5) continue;
        
        float ball = sdSphere(p, ballPositions[i], ballRadius);
        if (ball < min_dist) { 
            min_dist = ball; 
            hitType = 3; 
            ballIndex = i; 
        }
    }
    return min_dist;
}

// Расчет нормали
vec3 calcNormal(vec3 p, int hitType, int ballIndex) {
    // Для сфер используем идеально точную математическую нормаль
    if (hitType == 3 && ballIndex >= 0) {
        return normalize(p - ballPositions[ballIndex]);
    }
    // Для стола нормаль всегда смотрит строго вверх
    if (hitType == 1) {
        return vec3(0.0, 0.0, 1.0);
    }
    // Для бортов используем численный градиент
    const float h = 0.0005; 
    int t1, t2;
    vec2 k = vec2(1, -1);
    return normalize(
        k.xyy * sceneSDF(p + k.xyy * h, t1, t2, false) +
        k.yyx * sceneSDF(p + k.yyx * h, t1, t2, false) +
        k.yxy * sceneSDF(p + k.yxy * h, t1, t2, false) +
        k.xxx * sceneSDF(p + k.xxx * h, t1, t2, false)
    );
}

// UV развертка
vec2 sphereUV(vec3 normal) {
    float u = 0.75 - atan(normal.z, normal.x) / (2.0 * 3.14159265);
    float v = 0.5 + asin(normal.y) / 3.14159265;
    u -= floor(u);
    // Масштабируем текстуру, чтобы номер был крупнее
    return clamp(vec2(0.5) + (vec2(u, v) - 0.5) * 3.0, 0.0, 1.0);
}

// Освещение и тени
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float getSoftShadow(vec3 ro, vec3 rd, float mint, float maxt, float k) {
    float res = 1.0;
    float t = mint;
    // 32 итерации — баланс между качеством мягкой тени и FPS
    for (int i = 0; i < 32; ++i) {
        int ht, bi;
        float h = sceneSDF(ro + rd * t, ht, bi, true);
        if (h < HIT_THRESHOLD) return 0.0; // Точка полностью в тени
        res = min(res, k * h / t);        // k определяет мягкость края
        t += h;
        if (t > maxt) break;
    }
    return res;
}

// Ray marching
vec3 rayMarch(vec3 ro, vec3 rd) {
    vec3 accumulatedColor = vec3(0.0); // Итоговый цвет пикселя
    vec3 throughput = vec3(1.0);       // "Энергия" луча

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce) {
        float t = 0.01; // Начальный отступ луча от поверхности
        int hitType = 0; 
        int ballIndex = -1; 
        bool hit = false;

        // Поиск ближайшего столкновения
        for (int i = 0; i < MAX_STEP_COUNT; ++i) {
            vec3 p = ro + rd * t;
            float d = sceneSDF(p, hitType, ballIndex, false);
            
            // Если дистанция до объекта меньше порога — мы столкнулись
            if (abs(d) < HIT_THRESHOLD) { 
                hit = true; 
                break; 
            }
            t += abs(d); // Прыгаем вперед на безопасное расстояние
            
            // Если улетели слишком далеко — прекращаем поиск
            if (t > MAX_TRACE_DIST) break;
        }

        if (hit) {
            vec3 p = ro + rd * t;
            vec3 normal = calcNormal(p, hitType, ballIndex);
            
            // Проверяем, луч входит в объект или выходит из него
            bool entering = dot(rd, normal) < 0.0;
            vec3 hitNormal = entering ? normal : -normal;

            // Определение свойств материала в точке удара
            BallMaterial mat;
            if (hitType == 1) {      // Стол
                mat.baseColor = vec3(0.05, 0.35, 0.05); 
                mat.roughness = 0.5; 
                mat.transmission = 0.0; 
                mat.metallic = 0.0;
            } else if (hitType == 2) { // Борта
                mat.baseColor = vec3(0.15, 0.08, 0.04); 
                mat.roughness = 0.2; 
                mat.transmission = 0.0; 
                mat.metallic = 0.0;
            } else {                 // Шары
                mat = ballMaterials[ballIndex];
                if (mat.textureID >= 0) {
                    mat.baseColor = texture(ballTextures[mat.textureID], sphereUV(hitNormal)).rgb;
                }
            }

            // Расчет освещения (Прямой свет)
            vec3 V = -rd; // Вектор к камере
            vec3 L = normalize(pointLightPos - p); // Вектор к лампе
            
            float shadowMint = 0.03; // Минимальный зазор для луча тени
            float shadow = getSoftShadow(p + hitNormal * shadowMint, L, shadowMint, length(pointLightPos - p), 12.0);
            float NdotL = max(dot(hitNormal, L), 0.0);
            
            // Зеркальный блик (настраиваем "остроту" через pow)
            vec3 H = normalize(L + V);
            float spec = pow(max(dot(hitNormal, H), 0.0), 64.0) * 0.5 * shadow;
            
            // Если объект непрозрачный, добавляем диффузный цвет
            if (mat.transmission < 0.1) {
                accumulatedColor += throughput * (mat.baseColor * NdotL * shadow + spec * pointLightColor);
            } else {
                accumulatedColor += throughput * spec * pointLightColor; // На стекле только блик
            }

            // Вычисление направления для следующего "отскока" луча
            if (mat.transmission > 0.5) {
                // Преломление
                float eta = entering ? (1.0 / mat.refraction) : mat.refraction;
                vec3 refrDir = refract(rd, hitNormal, eta);
                
                if (length(refrDir) > 0.01) {
                    rd = refrDir;
                    ro = p + rd * SURFACE_BIAS; // Проталкиваем луч сквозь поверхность
                    throughput *= mat.baseColor; // Стекло окрашивает луч
                } else {
                    // Полное внутреннее отражение (если угол слишком острый)
                    rd = reflect(rd, hitNormal);
                    ro = p + hitNormal * SURFACE_BIAS;
                }
            } else {
                // Отражение
                rd = reflect(rd, hitNormal);
                ro = p + hitNormal * SURFACE_BIAS;
                
                float fresnel = 0.3; // Интенсивность отражения
                throughput *= fresnel; 
                
                // Если объект матовый и не металл, прекращаем отскоки
                if (mat.metallic < 0.1) break;
            }
        } else {
            // Если луч ни во что не попал — рисуем цвет неба
            accumulatedColor += throughput * vec3(0.5, 0.7, 1.0);
            break;
        }
    }
    return accumulatedColor;
}

void main() {
    // Нормализация координат пикселя
    vec2 uv = (gl_FragCoord.xy - resolution * 0.5) / resolution.y;
    
    // Построение луча из камеры
    vec3 rd = normalize(cameraRight * uv.x + cameraUp * uv.y + cameraView * tanFovHalf);
    
    // Запуск рендера
    vec3 color = rayMarch(cameraPos, rd);
    FragColor = vec4(color, 1.0);
}