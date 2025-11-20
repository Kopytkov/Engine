#pragma once
#include <cmath>
#include "material.h"
#include "math/vec_functions.h"
#include "scene.h"

struct Hit;
class Scene;

class MaterialPhong : public Material {
 private:
  RGB base_color_;
  float roughness_;  // шероховатость

 public:
  MaterialPhong(const RGB& color, float roughness);

  RGB albedo(const Hit& hit, const Scene& scene) const override;
  vec3 shade(const Hit& hit,
             const Scene& scene,
             const vec3& V,
             int depth) const override;
};
