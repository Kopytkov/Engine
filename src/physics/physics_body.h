#pragma once

#include <memory>
#include "math/vec_functions.h"
#include "physics_material.h"

class PhysicsBody {
 public:
  // Кинематические параметры
  vec3 position{0.0f};      // Позиция центра масс (м)
  vec3 velocity{0.0f};      // Линейная скорость (м/с)
  vec3 acceleration{0.0f};  // Линейное ускорение (м/с^2)

  // Динамические параметры
  vec3 forceAccum{0.0f};  // Сумма всех сил, действующих в текущем кадре (Н)

  // Вращательные параметры
  quat orientation;            // Текущий поворот (кватернион)
  vec3 angularVelocity{0.0f};  // Угловая скорость (рад/с) вокруг осей X,Y,Z
  vec3 torqueAccum{0.0f};      // Сумма вращающих моментов (Н*м)

  // Инерционные параметры
  float mass = 1.0f;        // Масса (кг)
  float invMass = 1.0f;     // Обратная масса (1/кг)
  float invInertia = 0.0f;  // Обратный момент инерции (1/(кг*м^2))

  // Свойства симуляции
  float linearDamping = 0.995f;  // Коэф. сопротивления среды для позиции
  float angularDamping = 0.98f;  // Коэф. сопротивления среды для вращения
  bool isStatic = false;         // Флаг статического объекта (неподвижен)

  std::shared_ptr<PhysicsMaterial> material;

  PhysicsBody();

  void IntegrateState(float deltaTime);
  void ApplyForce(const vec3& force);
  void ApplyForceAtPoint(const vec3& force, const vec3& point);
  void ClearForces();

  void SetMass(float m);
  void SetInertiaSphere(float radius);
  void SetInertiaBox(const vec3& halfExtents);

  void SetVelocity(const vec3& v);
  vec3 GetVelocity() const;

  void SetPosition(const vec3& p);
  vec3 GetPosition() const;
};
