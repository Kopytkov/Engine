#include "scene_object_sphere.h"

#include "math/vec_functions.h"

Sphere::Sphere(const vec3 &position, float r)
    : position_(position), radius_(r) {}

float Sphere::SDF(const vec3 &point) const {
  return length(point - position_) - radius_;
}