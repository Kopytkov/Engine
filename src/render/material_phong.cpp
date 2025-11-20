#include "material_phong.h"

MaterialPhong::MaterialPhong(const RGB& color, float roughness)
    : base_color_(color), roughness_(std::clamp(roughness, 0.0f, 1.0f)) {}

RGB MaterialPhong::albedo(const Hit& hit, const Scene& scene) const {
  return base_color_;
}

vec3 MaterialPhong::shade(const Hit& hit,
                          const Scene& scene,
                          const vec3& V,
                          int depth) const {
  vec3 baseColor = RGBtoVec3(base_color_);
  vec3 finalColor(0.0f);

  // Преобразуем roughness в shininess (блеск)
  float shininess = std::lerp(2.0f, 500.0f, 1.0f - roughness_);

  for (const auto& light : scene.GetLights()) {
    vec3 L = light->lightDirection(hit.position);
    vec3 N = hit.normal;

    float NdotL = dot(N, L);
    if (NdotL <= 0.0f) {
      continue;
    }

    float brightness = light->getBrightness();
    if (brightness <= 0.0f) {
      continue;
    }

    // Тени
    Ray shadow_ray(hit.position, L, 0);
    if (scene.GetHit(shadow_ray, kMaxDistance, hit.obj)) {
      continue;
    }

    vec3 lightColor = RGBtoVec3(light->getColor());
    vec3 radiance = lightColor * brightness;

    // Diffuse
    vec3 diffuse = baseColor * NdotL;

    // Specular (Blinn-Phong)
    vec3 H = normalize(L + V);
    float NdotH = std::max(0.0f, dot(N, H));
    float spec = std::pow(NdotH, shininess);
    vec3 specular = vec3(1.0f) * spec;  // белый блик

    vec3 Lo = (diffuse + specular) * radiance;
    finalColor += Lo;
  }

  return finalColor;
}
