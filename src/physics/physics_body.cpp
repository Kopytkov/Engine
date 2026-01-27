#include "physics_body.h"
#include "math/vec_functions.h"

PhysicsBody::PhysicsBody() {
  material = std::make_shared<PhysicsMaterial>();
  SetMass(1.0f);
}

void PhysicsBody::IntegrateState(float deltaTime) {
  if (isStatic || invMass == 0.0f) {
    return;
  }

  // --- Линейное движение ---
  vec3 resultingAcc = acceleration + (forceAccum * invMass);
  velocity = velocity + resultingAcc * deltaTime;
  position = position + velocity * deltaTime;

  // Эмуляция сопротивления воздуха (линейное)
  velocity = velocity * linearDamping;

  // --- Вращательное движение ---
  vec3 angularAcc = torqueAccum * invInertia;
  angularVelocity = angularVelocity + angularAcc * deltaTime;

  // Эмуляция сопротивления качению (угловое)
  angularVelocity = angularVelocity * angularDamping;

  // Интеграция ориентации с помощью кватернионов
  quat qVel(0.0f, angularVelocity[0], angularVelocity[1], angularVelocity[2]);
  quat spin = qVel * orientation * 0.5f;
  orientation = orientation + spin * deltaTime;

  // Нормализуем, иначе накопится ошибка и шар деформируется
  orientation = normalize(orientation);

  // Очистка сил для следующего кадра
  ClearForces();
}

// Приложение центральной силы (только движение)
void PhysicsBody::ApplyForce(const vec3& force) {
  if (!isStatic) {
    forceAccum = forceAccum + force;
  }
}

// Приложение силы в точке (движение + вращение)
void PhysicsBody::ApplyForceAtPoint(const vec3& force, const vec3& point) {
  if (isStatic) {
    return;
  }

  // Добавляем линейную силу
  forceAccum = forceAccum + force;

  // Вычисляем плечо силы
  vec3 r = point - position;

  // Момент силы
  torqueAccum = torqueAccum + cross(r, force);
}

// Очистка накопленных сил и моментов
void PhysicsBody::ClearForces() {
  forceAccum = vec3(0.0f);
  torqueAccum = vec3(0.0f);
}

void PhysicsBody::SetMass(float m) {
  mass = m;
  if (mass <= 0.0f) {
    invMass = 0.0f;
    isStatic = true;
    invInertia = 0.0f;
  } else {
    invMass = 1.0f / mass;
    isStatic = false;
    SetInertiaSphere(1.0f);
  }
}

void PhysicsBody::SetInertiaSphere(float radius) {
  if (isStatic) {
    return;
  }
  // I = (2/5) * m * r^2
  float I = 0.4f * mass * radius * radius;
  invInertia = (I != 0.0f) ? 1.0f / I : 0.0f;
}

void PhysicsBody::SetInertiaBox(const vec3& halfExtents) {
  if (isStatic) {
    return;
  }

  // Упрощенный расчет момента инерции для скалярного значения (аппроксимация)
  float w2 = halfExtents[0] * halfExtents[0];
  float h2 = halfExtents[1] * halfExtents[1];
  float d2 = halfExtents[2] * halfExtents[2];

  // Грубое приближение для float: среднее арифметическое инерций по осям
  float I_xx =
      (1.0f / 3.0f) * mass * (h2 + d2);  // 1/3 т.к. halfExtents, а не full size
  float I_yy = (1.0f / 3.0f) * mass * (w2 + d2);
  float I_zz = (1.0f / 3.0f) * mass * (w2 + h2);

  // Средняя инерция по трем осям
  float I_avg = (I_xx + I_yy + I_zz) / 3.0f;

  invInertia = (I_avg != 0.0f) ? 1.0f / I_avg : 0.0f;
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
