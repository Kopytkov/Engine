#pragma once
#include "control/property.h"
#include "scene_object.h"

class Box : public SceneObject, public PositionProperty {
 public:
  Box(const vec3& position, const vec3& vertex, std::unique_ptr<Material> mat);

  float SDF(const vec3& point) const override;
  vec3 getNormal(const vec3& point) const override;
  const Material& GetMaterial() const override;
  void UpdateUniforms(Shader& shader) const override;
  void SetRenderPosition(const vec3& position) override;
  vec3 GetRenderPosition() const override;

  vec3 GetHalfExtents() const;

  // Главный метод входа
  CollisionManifold ComputeCollision(
      const SceneObject* other,
      const PhysicsBody* myBody,
      const PhysicsBody* otherBody) const override;

  // Визитор для Sphere
  CollisionManifold ComputeCollisionWith(
      const Sphere* otherSphere,
      const PhysicsBody* myBody,
      const PhysicsBody* otherBody) const override;

  // Визитор для Box
  CollisionManifold ComputeCollisionWith(
      const Box* otherBox,
      const PhysicsBody* myBody,
      const PhysicsBody* otherBody) const override;

 private:
  vec3 vertex_;
  std::unique_ptr<Material> material_;
};
