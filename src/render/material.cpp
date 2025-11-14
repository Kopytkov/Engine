#include "material.h"
#include "render/raw_image.h"
#include "render/scene.h"

Material::Material() : texture(RawImage(1, 1)) {
  texture.getImage().SetPixel(0, 0, RGB{255, 255, 255});
  base_color = RGB{255, 255, 255};
}

Material::~Material() = default;

Material::Material(Texture& tex) : texture(tex), base_color{255, 255, 255} {}

Material::Material(float refl,
                   float tr,
                   float refract,
                   float rough,
                   float metal,
                   Texture& tex)
    : reflectivity(refl),
      transparency(tr),
      refraction(refract),
      roughness(rough),
      metallic(metal),
      texture(tex),
      base_color{255, 255, 255} {}

RGB Material::color(const Scene& scene,
                    vec3 position,
                    vec3 direction,
                    vec3 norm,
                    int numOfStep,
                    bool isOutside) const {
  return base_color;
}
