#pragma once

#include "math/vec.h"

class SceneObject {
public:
  virtual ~SceneObject() = default;

  virtual float SDF(const vec3 &point) const = 0;
};