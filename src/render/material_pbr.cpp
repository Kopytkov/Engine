#include "material_pbr.h"

MaterialPBR::MaterialPBR(const RGB& color,
                         float rough,
                         float metal,
                         float trans,
                         float refrac,
                         std::shared_ptr<RawImage> albedoImage)
    : base_color_(color),
      roughness_(std::clamp(rough, 0.03f, 1.0f)),
      metallic_(std::clamp(metal, 0.0f, 1.0f)),
      transmission_(std::clamp(trans, 0.0f, 1.0f)),
      refraction_(std::max(refrac, 1.0f)),
      albedo_image_(albedoImage) {}

// D_GGX — нормализованное распределение микрофасетов (Normal Distribution
// Function). Контролирует «плотность» ориентированных граней, направленных к
// наблюдателю.
float MaterialPBR::D_GGX(float NdotH, float alpha) {
  float a2 = alpha * alpha;
  float denom = (NdotH * NdotH) * (a2 - 1.0f) + 1.0f;
  return a2 / std::max(M_PI * denom * denom, 1e-6f);
}

// G_Smith — геометрический фактор (Geometry Function), моделирует взаимное
// заслонение микрофасетов при просмотре под острым углом
float MaterialPBR::G_SchlickGGX(float NdotX, float k) {
  return NdotX / (NdotX * (1.0f - k) + k);
}

float MaterialPBR::G_Smith(float NdotV, float NdotL, float k) {
  return G_SchlickGGX(NdotV, k) * G_SchlickGGX(NdotL, k);
}

// Эффект Френеля (при малых углах отражение усиливается)
vec3 MaterialPBR::FresnelSchlick(float cosTheta, const vec3& F0) {
  return F0 + (vec3(1.0f, 1.0f, 1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}

RGB MaterialPBR::albedo(const Hit& hit, const Scene&) const {
  if (!albedo_image_) {
    return base_color_;
  }

  vec3 n = normalize(hit.normal);

  // Вычисление UV-координат на основе нормали сферы
  float u = 0.75f - std::atan2(n[2], n[0]) / (2.0f * M_PI);
  float v = 0.5f + std::asin(n[1]) / M_PI;

  u = u - std::floor(u);
  v = std::clamp(v, 0.0f, 1.0f);

  // Масштабирование вокруг центра для уменьшения размера текстуры
  const float center_u = 0.5f;
  const float center_v = 0.5f;
  const float scale = 3.0f;  // для сжатия
  float u_scaled = center_u + (u - center_u) * scale;
  float v_scaled = center_v + (v - center_v) * scale;

  u_scaled = std::clamp(u_scaled, 0.0f, 1.0f);
  v_scaled = std::clamp(v_scaled, 0.0f, 1.0f);

  int x = std::min(int(u_scaled * albedo_image_->GetWidth()),
                   int(albedo_image_->GetWidth() - 1));

  int y = std::min(int(v_scaled * albedo_image_->GetHeight()),
                   int(albedo_image_->GetHeight() - 1));

  return albedo_image_->GetPixel(x, y);
}

vec3 MaterialPBR::shade(const Hit& hit,
                        const Scene& scene,
                        const vec3& V,
                        int depth) const {
  // Лимит глубины трассировки
  if (depth > 6) {
    return vec3(0.0f);
  }

  // Конвертируем цвет материала из RGB в vec3
  RGB base_rgb = albedo(hit, scene);
  vec3 baseColor = RGBtoVec3(base_rgb);

  vec3 N = normalize(hit.normal);
  vec3 I = -V;  // Падающий вектор (от глаза к точке)

  // Определяем, входим мы или выходим
  float NdotI = dot(N, I);
  bool isEntering = NdotI < 0.0f;

  // effectiveN всегда смотрит НАВСТРЕЧУ взгляду (для освещения)
  vec3 effectiveN = isEntering ? N : -N;
  float NdotV = std::max(0.0f, dot(effectiveN, V));

  // Для функции refract нормаль ТОЖЕ должна смотреть навстречу лучу I
  vec3 refractN = effectiveN;

  // Коэффициенты преломления
  float eta;
  if (isEntering) {
    eta = 1.0f / refraction_;  // Воздух -> Стекло
  } else {
    eta = refraction_;  // Стекло -> Воздух
  }

  // Bias для борьбы с кольцами и черными пятнами
  const float EPSILON = 0.002f;
  vec3 bias = effectiveN * EPSILON;

  //  Direct Lighting (Свет от ламп)
  vec3 F0 = vec3(0.04f);
  F0 = mix(F0, baseColor, metallic_);
  vec3 directLighting(0.0f);

  for (const auto& light : scene.GetLights()) {
    vec3 L = light->lightDirection(hit.position);
    float NdotL = std::max(0.0f, dot(effectiveN, L));
    float brightness = light->getBrightness();

    if (NdotL <= 0.0f || brightness <= 0.0f) {
      continue;
    }

    // Теневой луч
    Ray shadow_ray(hit.position + bias, L, 0);
    auto shadow_hit = scene.GetHit(shadow_ray, kMaxDistance, hit.obj);

    if (shadow_hit) {
      const Material& shadow_mat = shadow_hit->obj->GetMaterial();
      if (!shadow_mat.isTransparent()) {
        continue;
      }
    }

    vec3 H = normalize(L + V);
    float NdotH = std::max(0.0f, dot(effectiveN, H));
    float VdotH = std::max(0.0f, dot(V, H));

    // PBR Specular
    float alpha = roughness_ * roughness_;
    float D = D_GGX(NdotH, alpha);
    float k = (roughness_ + 1.0f) * (roughness_ + 1.0f) / 8.0f;
    float G = G_Smith(NdotV, NdotL, k);
    vec3 F_light = FresnelSchlick(VdotH, F0);

    vec3 specular = (D * G * F_light) / (4.0f * NdotV * NdotL + 1e-6f);
    vec3 kD =
        (vec3(1.0f) - F_light) * (1.0f - metallic_) * (1.0f - transmission_);

    vec3 radiance = RGBtoVec3(light->getColor()) * brightness;
    directLighting += (kD * baseColor / M_PI + specular) * radiance * NdotL;
  }

  // GI: отражение и преломление

  vec3 reflectionColor(0.0f);
  // Отражение (Reflection)
  {
    vec3 reflectDir = reflect(I, effectiveN);
    Ray reflectRay(hit.position + bias, reflectDir, 0);
    reflectionColor = scene.CastRay(reflectRay, depth + 1);
  }

  vec3 refractionColor(0.0f);
  vec3 F = FresnelSchlick(NdotV, F0);  // Френель для смешивания

  // Преломление (Refraction)
  if (transmission_ > 0.0f) {
    // Используем refractN, смотрящую навстречу I
    vec3 refractDir = refract(I, refractN, eta);

    if (length(refractDir) < 0.01f) {
      // Полное внутреннее отражение (TIR)
      refractionColor = reflectionColor;
      F = vec3(1.0f);
    } else {
      // Луч проходит сквозь
      // Сдвигаем точку старта В СТОРОНУ ПРЕЛОМЛЕНИЯ (normalize(refractDir))
      vec3 startPoint = hit.position + normalize(refractDir) * EPSILON;

      Ray nextRay(startPoint, normalize(refractDir), 0);
      refractionColor = scene.CastRay(nextRay, depth + 1) * baseColor;
    }
  }

  // Смешиваем результат
  vec3 finalColor = directLighting * (1.0f - transmission_);

  vec3 glassPart = reflectionColor * F + refractionColor * (vec3(1.0f) - F);

  return finalColor + glassPart * transmission_;
}

bool MaterialPBR::isTransparent() const {
  return transmission_ > 0.9f;
}
