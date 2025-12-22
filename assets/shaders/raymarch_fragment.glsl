#version 330 core
out vec4 FragColor;

// Камера
uniform vec3 cameraPos;
uniform vec3 cameraView;
uniform vec3 cameraUp;
uniform vec3 cameraRight;
uniform float tanFovHalf;
uniform vec2 resolution;

// Освещение
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float pointLightBrightness;
uniform vec3 globalLightDir;
uniform vec3 globalLightColor;
uniform float globalLightBrightness;

// Текстуры и материалы шаров
uniform sampler2D ballTextures[16];

struct BallMaterial {
    vec3 baseColor;
    float roughness;
    float metallic;
    float transmission;
    float refraction;
    int textureID; // -1 = нет текстуры
};
uniform BallMaterial ballMaterials[16];

// Динамические параметры
uniform vec3 ballPositions[16];
uniform int ballCount;
uniform float ballRadius;

// SDF функции
float sdSphere(vec3 p, vec3 c, float r) {
    return length(p - c) - r;
}

float sdBox(vec3 p, vec3 c, vec3 b) {
    vec3 q = abs(p - c) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

// Сцена
float sceneSDF(vec3 p, out int hitType, out int ballIndex) {
    hitType = 0;
    ballIndex = -1;
    float d = 1e10;

    // Стол
    float table = sdBox(p, vec3(0.0, 2.7, -1.0), vec3(8.85, 5.85, 1.0));
    if (table < d) { d = table; hitType = 1; }

    // Борты
    float borders = 1e10;
    borders = min(borders, sdBox(p, vec3(-9.0, 2.7, 0.5), vec3(0.15, 6.0, 1.5)));
    borders = min(borders, sdBox(p, vec3( 9.0, 2.7, 0.5), vec3(0.15, 6.0, 1.5)));
    borders = min(borders, sdBox(p, vec3(0.0, -3.3, 0.5), vec3(9.0, 0.15, 1.5)));
    borders = min(borders, sdBox(p, vec3(0.0,  8.7, 0.5), vec3(9.0, 0.15, 1.5)));
    if (borders < d) { d = borders; hitType = 2; }

    // Шары
    for (int i = 0; i < ballCount; ++i) {
        float ball = sdSphere(p, ballPositions[i], ballRadius);
        if (ball < d) {
            d = ball;
            hitType = 3;
            ballIndex = i;
        }
    }

    return d;
}

vec3 calcNormal(vec3 p, int hitType, int ballIndex) {
    const float h = 0.001;
    int t1, t2;
    return normalize(vec3(
        sceneSDF(p + vec3(h, 0, 0), t1, t2) - sceneSDF(p - vec3(h, 0, 0), t1, t2),
        sceneSDF(p + vec3(0, h, 0), t1, t2) - sceneSDF(p - vec3(0, h, 0), t1, t2),
        sceneSDF(p + vec3(0, 0, h), t1, t2) - sceneSDF(p - vec3(0, 0, h), t1, t2)
    ));
}

// UV для сферы
vec2 sphereUV(vec3 n) {
    float u = 0.75 - atan(n.z, n.x) / (2.0 * 3.14159265);
    float v = 0.5 + asin(n.y) / 3.14159265;
    u -= floor(u);
    v = clamp(v, 0.0, 1.0);
    vec2 center = vec2(0.5);
    float scale = 3.0;
    vec2 scaled = center + (vec2(u, v) - center) * scale;
    return clamp(scaled, 0.0, 1.0);
}

// PBR функции
float D_GGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / max(3.14159265 * denom * denom, 1e-6);
}

float G_SchlickGGX(float NdotV, float roughness) {
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float G_Smith(float NdotV, float NdotL, float roughness) {
    return G_SchlickGGX(NdotV, roughness) * G_SchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float softShadow(vec3 ro, vec3 rd, float mint, float maxt, float k) {
    float res = 1.0;
    for (float t = mint; t < maxt;) {
        int ht, bi;
        float h = sceneSDF(ro + rd * t, ht, bi);
        if (h < 0.001) return 0.0;
        res = min(res, k * h / t);
        t += h;
    }
    return res;
}

// Ray marching
vec3 rayMarch(vec3 ro, vec3 rd) {
    vec3 accumulatedColor = vec3(0.0);
    vec3 throughput = vec3(1.0);
    const int MAX_BOUNCES = 4;

    for (int bounce = 0; bounce < MAX_BOUNCES; ++bounce) {
        float t = 0.0;
        int hitType = 0, ballIndex = -1;
        bool hit = false;

        for (int i = 0; i < 256; ++i) {
            vec3 p = ro + rd * t;
            float d = sceneSDF(p, hitType, ballIndex);
            if (d < 0.001) {
                hit = true;
                vec3 normal = calcNormal(p, hitType, ballIndex);
                vec3 V = -rd;

                vec3 albedo = vec3(0.05, 0.35, 0.05);
                float roughness = 0.5;
                float metallic = 0.0;
                float transmission = 0.0;
                float refraction = 1.5;
                int texID = -1;

                if (hitType == 2) {
                    albedo = vec3(0.4, 0.2, 0.1);
                } else if (hitType == 3) {
                    BallMaterial mat = ballMaterials[ballIndex];
                    albedo = mat.baseColor;
                    roughness = mat.roughness;
                    metallic = mat.metallic;
                    transmission = mat.transmission;
                    refraction = mat.refraction;
                    texID = mat.textureID;
                    if (texID >= 0) {
                        vec2 uv = sphereUV(normal);
                        albedo = texture(ballTextures[texID], uv).rgb;
                    }
                }

                vec3 F0 = mix(vec3(0.04), albedo, metallic);
                vec3 direct = vec3(0.0);

                // Point light
                vec3 L = normalize(pointLightPos - p);
                float NdotL = max(dot(normal, L), 0.0);
                if (NdotL > 0.0) {
                    float shadow = softShadow(p + normal * 0.01, L, 0.1, length(pointLightPos - p), 8.0);
                    vec3 H = normalize(L + V);
                    float NdotH = max(dot(normal, H), 0.0);
                    float VdotH = max(dot(V, H), 0.0);
                    float alpha = roughness * roughness;
                    float D = D_GGX(NdotH, alpha);
                    float G = G_Smith(max(dot(normal, V), 0.0), NdotL, roughness);
                    vec3 F = FresnelSchlick(VdotH, F0);
                    vec3 specular = (D * G * F) / (4.0 * max(dot(normal, V), 0.0) * NdotL + 1e-6);
                    vec3 kD = (1.0 - F) * (1.0 - metallic);
                    vec3 radiance = pointLightColor * pointLightBrightness;
                    direct += (kD * albedo / 3.14159265 + specular) * radiance * NdotL * shadow;
                }

                // Global light
                vec3 Lg = -normalize(globalLightDir);
                float NdotLg = max(dot(normal, Lg), 0.0);
                if (NdotLg > 0.0) {
                    vec3 H = normalize(Lg + V);
                    float NdotH = max(dot(normal, H), 0.0);
                    float VdotH = max(dot(V, H), 0.0);
                    float alpha = roughness * roughness;
                    float D = D_GGX(NdotH, alpha);
                    float G = G_Smith(max(dot(normal, V), 0.0), NdotLg, roughness);
                    vec3 F = FresnelSchlick(VdotH, F0);
                    vec3 specular = (D * G * F) / (4.0 * max(dot(normal, V), 0.0) * NdotLg + 1e-6);
                    vec3 kD = (1.0 - F) * (1.0 - metallic);
                    vec3 radiance = globalLightColor * globalLightBrightness;
                    direct += (kD * albedo / 3.14159265 + specular) * radiance * NdotLg;
                }

                accumulatedColor += throughput * direct;

                rd = reflect(rd, normal);
                ro = p + normal * 0.01;
                throughput *= mix(vec3(0.3), vec3(0.8), metallic);

                if (transmission > 0.0) {
                    float eta = 1.0 / refraction;
                    vec3 refractDir = refract(rd, normal, eta);
                    if (length(refractDir) > 0.01) {
                        rd = refractDir;
                        ro = p - normal * 0.01;
                        throughput *= vec3(0.9);
                    }
                }

                break;
            }

            t += d;
            if (t > 200.0) {
                accumulatedColor += throughput * vec3(0.5, 0.7, 1.0);
                return accumulatedColor;
            }
        }

        if (!hit) {
            accumulatedColor += throughput * vec3(0.5, 0.7, 1.0);
            return accumulatedColor;
        }
    }

    return accumulatedColor;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - resolution * 0.5) / resolution.y;
    vec3 rd = normalize(cameraRight * uv.x + cameraUp * uv.y + cameraView * tanFovHalf);
    vec3 color = rayMarch(cameraPos, rd);
    FragColor = vec4(color, 1.0);
}