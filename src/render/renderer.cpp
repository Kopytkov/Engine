#include "renderer.h"
#include "math/vec_functions.h"

void Renderer::Render(const Camera &camera, const Scene &scene,
                      RawImage &out_image) {
  for (uint32_t x = 0; x < out_image.GetWidth(); ++x) {
    for (uint32_t y = 0; y < out_image.GetHeight(); ++y) {
      auto [imgx, imgy, ray] = camera.GetRay(x, y);
      
      if (auto hit = scene.GetHit(ray); hit) {
        RGB color{0, 0, 0};

        for(const auto &light : scene.GetLights()){
          color = stretchRGB(color, RGB{255, 255, 255}*dot(hit->normal,  light->lightDirection(hit->position)));
        }

        out_image.SetPixel(imgx, imgy, color);
      }
    }
  }
}