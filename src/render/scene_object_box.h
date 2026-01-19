#pragma once
#include "control/property.h"
#include "scene_object.h"

class Box : public SceneObject, public PositionProperty {
 public:
  Box(const vec3& position, const vec3& vertex, std::unique_ptr<Material> mat);

  // Методы для рендеринга
  float SDF(const vec3& point) const override;
  vec3 getNormal(const vec3& point) const override;
  const Material& GetMaterial() const override;
  void UpdateUniforms(Shader& shader) const override;

  // Методы для физики
  vec3 GetPosition() const override;
  void SetPosition(const vec3& position) override;

 private:
  vec3 vertex_;
  std::unique_ptr<Material> material_;
};
