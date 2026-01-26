#include "physics_body.h"

PhysicsBody::PhysicsBody() {
  material = std::make_shared<PhysicsMaterial>();
}

void PhysicsBody::IntegrateState(float deltaTime) {
  if (isStatic) {
    return;  // Статические объекты не двигаются
  }

  // Интеграция скорости
  velocity = velocity + acceleration * deltaTime;

  // Интеграция позиции
  position = position + velocity * deltaTime;

  // Сброс ускорения (силы применяются заново каждый кадр)
  acceleration = vec3(0.0f);
}

void PhysicsBody::SetVelocity(const vec3& v) {
  if (!isStatic) {
    velocity = v;
  }
}

vec3 PhysicsBody::GetVelocity() const {
  return velocity;
}

void PhysicsBody::SetPosition(const vec3& p) {
  position = p;
}

vec3 PhysicsBody::GetPosition() const {
  return position;
}
