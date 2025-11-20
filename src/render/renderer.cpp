#include "renderer.h"
#include "math/vec_functions.h"

void Renderer::Render(const Camera& camera,
                      const Scene& scene,
                      RawImage& out_image) {
  for (uint32_t x = 0; x < out_image.GetWidth(); ++x) {
    for (uint32_t y = 0; y < out_image.GetHeight(); ++y) {
      auto [imgx, imgy, ray] = camera.GetRay(x, y);

      vec3 finalColor = scene.CastRay(ray, 0);

      RGB outRGB = Vec3toRGB(applyGamma(reinhardTonemap(finalColor)));
      out_image.SetPixel(imgx, imgy, outRGB);
    }
  }
}
