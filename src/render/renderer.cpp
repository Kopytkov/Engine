#include "renderer.h"
#include "math/vec_functions.h"

void Renderer::Render(const Camera& camera,
                      const Scene& scene,
                      RawImage& out_image) {
  for (uint32_t x = 0; x < out_image.GetWidth(); ++x) {
    for (uint32_t y = 0; y < out_image.GetHeight(); ++y) {
      auto [imgx, imgy, ray] = camera.GetRay(x, y);

      if (auto hit_opt = scene.GetHit(ray); hit_opt) {
        const Hit& hit = *hit_opt;
        const Material& mat = hit.obj->GetMaterial();

        // Цвет из материала
        RGB base = mat.color(scene, hit.position, -ray.direction, hit.normal,
                             ray.numOfStep, true);

        // Освещение с тенями
        RGB final{0, 0, 0};
        for (const auto& light : scene.GetLights()) {
          vec3 L = light->lightDirection(hit.position);
          float NdotL = std::max(0.0f, dot(hit.normal, L));

          // // Если свет не попадает на поверхность — пропускаем
          if (NdotL > 0.0f) {
            // Смещение от поверхности (bias) — избегаем self-intersection
            vec3 shadow_origin = hit.position + hit.normal * 0.1f;
            // Shadow ray: от точки к свету
            Ray shadow_ray(shadow_origin, L, 0);

            // Проверка тени: есть ли препятствие между точкой и светом?
            if (auto shadow_hit = scene.GetHit(shadow_ray, hit.obj);
                !shadow_hit) {
              // Вклад света в итоговый цвет
              RGB contrib = base * NdotL;

              // Умножаем на цвет света (RGB * RGB)
              contrib = contrib * light->getColor();

              // Умножаем на яркость (RGB * float)
              contrib = contrib * light->getBrightness();

              final = stretchRGB(final, contrib);
            }
          }
        }

        out_image.SetPixel(imgx, imgy, final);
      }
    }
  }
}
