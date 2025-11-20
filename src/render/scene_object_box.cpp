#include "scene_object_box.h"
#include "math/vec_functions.h"

Box::Box(const vec3& position, const vec3& vertex, const Material& mat)
    : vertex_(abs(vertex)), material_(mat) {
      SetPosition(position);
    }

float Box::SDF(const vec3& point) const {
  vec3 q = abs(point - GetPosition()) - vertex_;
  return length(max(q, 0.0f)) +
         std::min(std::max(q[0], std::max(q[1], q[2])), 0.0f);
}

vec3 Box::getNormal(const vec3& point) const {
  vec3 q = abs(point - GetPosition()) - vertex_;
  vec3 q1 = normalize(max(q, 0.0f) +
                      clamp(q, std::max(q[0], std::max(q[1], q[2])), 0.0f));
  vec3 q2 = point - GetPosition();
  vec3 res = q1 * vec3(-1.0f + 2 * int(q2[0] > 0), -1.0f + 2 * int(q2[1] > 0),
                       -1.0f + 2 * int(q2[2] > 0));
  return res;
}

const Material& Box::GetMaterial() const {
  return material_;
}
