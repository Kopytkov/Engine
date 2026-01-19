#pragma once
#include "material.h"
#include "math/vec_functions.h"
#include "scene.h"

struct Hit;
class Scene;

class MaterialPBR : public Material {
 private:
  RGB base_color_;           // базовый цвет
  float roughness_;          // шероховатость
  float metallic_;           // металличность
  float transmission_;       // светопропускание
  float refraction_;         // преломление
  std::string textureName_;  // имя текстуры

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
              const std::string& texName = "");

  RGB albedo(const Hit& hit, const Scene& scene) const override;

  vec3 shade(const Hit& hit,
             const Scene& scene,
             const vec3& V,
             int depth) const override;

  bool isTransparent() const override;

  const RGB& getBaseColor() const { return base_color_; }
  float getRoughness() const { return roughness_; }
  float getMetallic() const { return metallic_; }
  float getTransmission() const { return transmission_; }
  float getRefraction() const { return refraction_; }
  const std::string& getTextureName() const { return textureName_; }
};
