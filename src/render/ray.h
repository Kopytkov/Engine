#pragma once

#include "math/vec.h"

inline constexpr float kMinDistance = 0.001f;
inline constexpr uint32_t kMaxStep = 300;
inline constexpr float kMaxDistance = 1500.0f;

class Ray {
public:
  vec3 position;
  vec3 direction;
  uint32_t numOfStep = 0;

  Ray(const vec3 &pos, const vec3 &dir, int n = 0);
};
