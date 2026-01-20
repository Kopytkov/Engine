#pragma once
#include "control/property.h"
#include "scene_object.h"

class Sphere : public SceneObject, public PositionProperty {
 public:
  Sphere(const vec3& position, float r, std::unique_ptr<Material> mat);

  // Методы для рендеринга
  float SDF(const vec3& point) const override;
  vec3 getNormal(const vec3& point) const override;
  const Material& GetMaterial() const override;
  void UpdateUniforms(Shader& shader) const override;

  // Методы для физики
  vec3 GetPosition() const override;
  void SetPosition(const vec3& position) override;
  float GetRadius() const;

 private:
  float radius_;
  std::unique_ptr<Material> material_;
};
