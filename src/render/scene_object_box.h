#pragma once
#include "scene_object.h"

class Box : public SceneObject {
 public:
  Box(const vec3& position, const vec3& vertex, std::unique_ptr<Material> mat);
  float SDF(const vec3& point) const override;
  vec3 getNormal(const vec3& point) const override;
  const Material& GetMaterial() const override;

 private:
  vec3 position_;
  vec3 vertex_;
  std::unique_ptr<Material> material_;
};
