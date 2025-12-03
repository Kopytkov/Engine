#pragma once
#include "material.h"
#include "math/vec_functions.h"
#include "scene.h"

struct Hit;
class Scene;

class MaterialPBR : public Material {
 private:
  RGB base_color_;
  float roughness_;     // шероховатость
  float metallic_;      // металличность
  float transmission_;  // светопропускание
  float refraction_;    // преломление
  std::shared_ptr<RawImage> albedo_image_;

  static float D_GGX(float NdotH, float alpha);
  static float G_SchlickGGX(float NdotX, float k);
  static float G_Smith(float NdotV, float NdotL, float k);
  static vec3 FresnelSchlick(float VdotH, const vec3& F0);

 public:
  MaterialPBR(const RGB& color = RGB{255, 255, 255},
              float rough = 0.18f,
              float metal = 0.0f,
              float trans = 0.0f,
              float refrac = 1.5f,
              std::shared_ptr<RawImage> albedoImage = nullptr);

  RGB albedo(const Hit& hit, const Scene& scene) const override;

  vec3 shade(const Hit& hit,
             const Scene& scene,
             const vec3& V,
             int depth) const override;

  bool isTransparent() const override;
};
