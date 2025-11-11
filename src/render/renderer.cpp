#include "renderer.h"
#include "math/vec_functions.h"

void Renderer::Render(const Camera& camera,
                      const Scene& scene,
                      RawImage& out_image) {
  for (uint32_t x = 0; x < out_image.GetWidth(); ++x) {
    for (uint32_t y = 0; y < out_image.GetHeight(); ++y) {
      auto [imgx, imgy, ray] = camera.GetRay(x, y);

      Ray primary_ray = ray;
      primary_ray.numOfStep = 0;

      // Проверяем пересечение первичного луча со сценой
      if (auto hit_opt = scene.GetHit(primary_ray); hit_opt) {
        const Hit& hit = *hit_opt;
        const Material& mat = hit.obj->GetMaterial();

        // Базовый цвет материала (альбедо) в RGB
        RGB base_rgb = mat.color(scene, hit.position, -primary_ray.direction,
                                 hit.normal, primary_ray.numOfStep, true);

        vec3 baseColor = RGBtoVec3(base_rgb);  // переводим в vec3
        vec3 finalColor(0.0f);  // финальный линейный цвет пикселя
        vec3 V = normalize(-primary_ray.direction);  // направление взгляда

        for (const auto& light : scene.GetLights()) {
          vec3 L = light->lightDirection(hit.position);
          float NdotL = std::max(0.0f, dot(hit.normal, L));
          float brightness = light->getBrightness();

          // Если свет не попадает на поверхность — пропускаем
          if (NdotL <= 0.0f || brightness <= 0.0f) {
            continue;
          }

          // Проверяем, не перекрыт ли источник света (тень)
          Ray shadow_ray(hit.position, L, 0);
          if (auto shadow_hit = scene.GetHit(shadow_ray, kMaxDistance, hit.obj);
              shadow_hit) {
            continue;
          }

          // === модель Cook–Torrance (GGX) ===

          vec3 N = hit.normal;        // нормаль к поверхности
          vec3 H = normalize(L + V);  // полувектор между L и V
          vec3 lightColor = RGBtoVec3(light->getColor());
          vec3 radiance = lightColor * brightness;  // энергия источника света

          // Косинусы углов
          float NdotV = std::max(0.0f, dot(N, V));
          float NdotH = std::max(0.0f, dot(N, H));
          float VdotH = std::max(0.0f, dot(V, H));

          if (NdotV <= 0.0f) {
            continue;  // пиксель не виден наблюдателю
          }

          // 1. Извлечение параметров материала
          float roughness = std::clamp(mat.roughness, 0.03f, 1.0f);
          float metallic = std::clamp(mat.metallic, 0.0f, 1.0f);
          float alpha =
              roughness * roughness;  // Параметр микрофасетной функции

          // 2. Микрофасетные функции BRDF Cook–Torrance
          float D = D_GGX(NdotH, alpha);

          // Приближение Schlick (учитывает самозатенение микрофасетов)
          float k = (roughness + 1.0f);
          k = (k * k) / 8.0f;
          float G = G_Smith(NdotV, NdotL, k);

          // 3. Fresnel — закон отражения от микрофасета
          vec3 F0 = vec3(0.04f);  // базовое отражение диэлектрика

          // Интерполяция между диэлектриком и металлом
          F0 = F0 * (1.0f - metallic) + baseColor * metallic;
          vec3 F = FresnelSchlick(VdotH, F0);

          // 4. Собираем BRDF
          vec3 numerator = D * G * F;
          float epsilon = 1e-6f;
          float denom = 4.0f * NdotV * NdotL + epsilon;
          // Зеркальная энергия
          vec3 specular = numerator * (1.0f / denom);

          // 5. Диффузная часть (только для неметаллов)
          vec3 kd =
              (vec3(1.0f) - F) * (1.0f - metallic);  // доля рассеянного света

          // Диффузная часть — по ламбертовскому закону
          vec3 diffuse = kd * baseColor * (1.0f / static_cast<float>(M_PI));

          // 6. Финальный вклад света
          vec3 Lo = (kd * diffuse + specular) * radiance * NdotL;

          // Суммируем с другими источниками
          finalColor += Lo;
        }
        // Постобработка (тонмаппинг + Гамма-коррекция)
        RGB outRGB = Vec3toRGB(applyGamma(reinhardTonemap(finalColor), 2.2f));
        out_image.SetPixel(imgx, imgy, outRGB);
      }
    }
  }
}
