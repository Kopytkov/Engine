#include "scene_object_sphere.h"
#include "math/vec_functions.h"
#include "physics/physics_body.h"
#include "scene_object_box.h"

Sphere::Sphere(const vec3& position, float r, std::unique_ptr<Material> mat)
    : radius_(r), material_(std::move(mat)) {
  SetPosition(position);
}

float Sphere::SDF(const vec3& point) const {
  return length(point - GetPosition()) - radius_;
}

vec3 Sphere::getNormal(const vec3& point) const {
  return normalize(point - GetPosition());
}

const Material& Sphere::GetMaterial() const {
  return *material_.get();
}

void Sphere::UpdateUniforms(Shader& shader) const {}

void Sphere::SetRenderPosition(const vec3& position) {
  SetPosition(position);
}

vec3 Sphere::GetRenderPosition() const {
  return GetPosition();
}

float Sphere::GetRadius() const {
  return radius_;
}

CollisionManifold Sphere::ComputeCollision(const SceneObject* other,
                                           const PhysicsBody* myBody,
                                           const PhysicsBody* otherBody) const {
  return other->ComputeCollisionWith(this, otherBody, myBody);
}

// Sphere vs Sphere
CollisionManifold Sphere::ComputeCollisionWith(
    const Sphere* otherSphere,
    const PhysicsBody* myBody,
    const PhysicsBody* otherBody) const {
  CollisionManifold result;
  vec3 posSelf = myBody->GetPosition();
  vec3 posOther = otherBody->GetPosition();
  vec3 delta = posOther - posSelf;

  float dist = length(delta);
  float radiusSum = this->GetRadius() + otherSphere->GetRadius();

  if (dist < radiusSum) {
    result.isColliding = true;
    result.depth = radiusSum - dist;
    // Если центры совпадают, выбираем произвольную ось Y
    result.normal = (dist > 1e-6f) ? normalize(delta) : vec3(0, 1, 0);
    // Точка контакта лежит посередине между поверхностями (упрощенно)
    result.contactPoint = posSelf + result.normal * this->GetRadius();
  }
  return result;
}

// Sphere vs Box
CollisionManifold Sphere::ComputeCollisionWith(
    const Box* otherBox,
    const PhysicsBody* myBody,
    const PhysicsBody* otherBody) const {
  CollisionManifold res =
      otherBox->ComputeCollisionWith(this, otherBody, myBody);
  res.normal = -res.normal;  // Инвертируем нормаль
  return res;
}
