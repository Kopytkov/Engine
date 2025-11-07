#pragma once

#include "math/vec.h"



class Ray {
 public:
  vec3 position;
  vec3 direction;
  uint32_t numOfStep = 0;

  Ray(const vec3& pos, const vec3& dir, int n = 0);
};
