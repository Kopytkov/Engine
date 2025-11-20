#include "scene_object_sphere.h"
#include "math/vec_functions.h"

Sphere::Sphere(const vec3& position, float r, const Material& mat)
    : radius_(r), material_(mat) {
      SetPosition(position);
    }

float Sphere::SDF(const vec3& point) const {
  return length(point - GetPosition()) - radius_;
}

vec3 Sphere::getNormal(const vec3& point) const {
  return normalize(point - GetPosition());
}

const Material& Sphere::GetMaterial() const {
  return material_;
}
