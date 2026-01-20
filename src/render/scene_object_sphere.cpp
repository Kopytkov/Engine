#include "scene_object_sphere.h"
#include "math/vec_functions.h"

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

vec3 Sphere::GetPosition() const {
  return PositionProperty::GetPosition();
}

void Sphere::SetPosition(const vec3& position) {
  PositionProperty::SetPosition(position);
}

float Sphere::GetRadius() const {
  return radius_;
}
