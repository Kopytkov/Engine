#pragma once

#include <memory>
#include "math/vec.h"
#include "physics_material.h"

class PhysicsBody {
 public:
  vec3 position{0.0f};
  vec3 velocity{0.0f};
  vec3 acceleration{0.0f};
  float mass = 1.0f;
  bool isStatic = false;  // Флаг для статических объектов

  std::shared_ptr<PhysicsMaterial> material;

  PhysicsBody();

  void IntegrateState(float deltaTime);

  void SetVelocity(const vec3& v);
  vec3 GetVelocity() const;

  void SetPosition(const vec3& p);
  vec3 GetPosition() const;
};
