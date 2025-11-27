#pragma once

#include "math/vec.h"

class PositionProperty {
 private:
  vec3 position_;

 public:
  const vec3& GetPosition() const;

  void SetPosition(const vec3& position);
};
