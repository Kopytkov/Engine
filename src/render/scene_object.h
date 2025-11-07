#pragma once

#include "material.h"
#include "math/vec.h"

class SceneObject {
 public:
  virtual ~SceneObject() = default;

  virtual float SDF(const vec3& point) const = 0;

  virtual vec3 getNormal(const vec3& point) const = 0;

  virtual const Material& GetMaterial() const = 0;
};
