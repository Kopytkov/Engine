#include "physics_engine.h"
#include <algorithm>
#include <cmath>
#include "math/vec_functions.h"

constexpr float kFrictionThreshold =
    1e-4f;  // Порог касательной скорости для трения
constexpr float kRestingVelocityThreshold =
    0.05f;  // Порог нормальной скорости для "залипания"

void ResolveCollision(PhysicsBody* bodyA,
                      PhysicsBody* bodyB,
                      const CollisionManifold& manifold) {
  // Разделение объектов (Positional Correction)
  // Физически раздвигаем объекты, чтобы они не находились друг в друге.

  const float percent = 0.8f;  // Процент исправления (мягкое разделение)
  const float slop = 0.01f;    // Допуск проникновения (защита от дрожания)

  // Вычисляем обратную массу (для законов Ньютона)
  float invMassA = bodyA->invMass;
  float invMassB = bodyB->invMass;
  float invMassSum = invMassA + invMassB;

  if (invMassSum == 0.0f) {
    return;  // Оба объекта статичны
  }

  // Вектор коррекции: насколько нужно раздвинуть объекты вдоль нормали
  vec3 correction = manifold.normal * (std::max(manifold.depth - slop, 0.0f) /
                                       invMassSum * percent);

  if (!bodyA->isStatic) {
    bodyA->SetPosition(bodyA->GetPosition() + correction * invMassA);
  }
  if (!bodyB->isStatic) {
    bodyB->SetPosition(bodyB->GetPosition() - correction * invMassB);
  }

  // Импульс (Отскок)
  vec3 rA = manifold.contactPoint - bodyA->GetPosition();
  vec3 rB = manifold.contactPoint - bodyB->GetPosition();

  vec3 velA = bodyA->GetVelocity() + cross(bodyA->angularVelocity, rA);
  vec3 velB = bodyB->GetVelocity() + cross(bodyB->angularVelocity, rB);

  vec3 rv = velA - velB;
  float velAlongNormal = dot(rv, manifold.normal);

  // Если объекты удаляются друг от друга, не трогаем их
  if (velAlongNormal > 0) {
    return;
  }

  // Вычисляем упругость (берем минимальную из двух)
  float e =
      std::min(bodyA->material->restitution, bodyB->material->restitution);

  // Стабилизация (Resting Contact)
  if (velAlongNormal > -kRestingVelocityThreshold) {
    e = 0.0f;
  }

  // Вычисляем импульсный множитель
  float j = -(1.0f + e) * velAlongNormal;
  j /= invMassSum;

  // Применяем импульс к скоростям
  vec3 impulse = manifold.normal * j;

  if (!bodyA->isStatic) {
    bodyA->SetVelocity(bodyA->GetVelocity() + impulse * invMassA);
  }
  if (!bodyB->isStatic) {
    bodyB->SetVelocity(bodyB->GetVelocity() - impulse * invMassB);
  }

  // Трение (Friction Impulse) и Вращение
  velA = bodyA->GetVelocity() + cross(bodyA->angularVelocity, rA);
  velB = bodyB->GetVelocity() + cross(bodyB->angularVelocity, rB);
  rv = velA - velB;

  vec3 tangent = rv - (manifold.normal * dot(rv, manifold.normal));
  float tangentLen = length(tangent);

  if (tangentLen > kFrictionThreshold) {
    tangent = tangent * (1.0f / tangentLen);

    // Импульс, необходимый для полной остановки скольжения
    float jt = -dot(rv, tangent);
    jt /= invMassSum;

    // Коэффициент трения
    float mu = std::sqrt(bodyA->material->friction * bodyA->material->friction +
                         bodyB->material->friction * bodyB->material->friction);

    float maxFriction = j * mu;

    // Кулоново трение: либо останавливаем (jt), либо скользим (maxFriction)
    float frictionImpulseMagnitude =
        (std::abs(jt) < maxFriction) ? jt
                                     : ((jt > 0 ? 1.0f : -1.0f) * maxFriction);

    vec3 frictionImpulse = tangent * frictionImpulseMagnitude;

    // Линейный импульс трения
    if (!bodyA->isStatic) {
      bodyA->SetVelocity(bodyA->GetVelocity() + frictionImpulse * invMassA);
    }
    if (!bodyB->isStatic) {
      bodyB->SetVelocity(bodyB->GetVelocity() - frictionImpulse * invMassB);
    }

    // Вращающий момент от трения
    if (!bodyA->isStatic) {
      vec3 torqueImpulse = cross(rA, frictionImpulse);
      bodyA->angularVelocity =
          bodyA->angularVelocity + torqueImpulse * bodyA->invInertia;
    }
    if (!bodyB->isStatic) {
      vec3 torqueImpulse = cross(rB, -frictionImpulse);
      bodyB->angularVelocity =
          bodyB->angularVelocity + torqueImpulse * bodyB->invInertia;
    }
  }
}

void PhysicsEngine::Update(Scene& scene, float deltaTime) {
  // Применяем внешние силы
  ApplyGravity(scene);

  // Решаем коллизии
  ProcessCollisions(scene, deltaTime);

  // Интегрируем (Двигаем объекты)
  IntegrateBodies(scene, deltaTime);
}

void PhysicsEngine::ApplyGravity(Scene& scene) {
  for (auto& entity : scene.GetEntities()) {
    if (entity->body && !entity->body->isStatic) {
      // F = m * g
      vec3 gravityForce = gravity * entity->body->mass;
      entity->body->ApplyForce(gravityForce);
    }
  }
}

void PhysicsEngine::IntegrateBodies(Scene& scene, float deltaTime) {
  for (auto& entity : scene.GetEntities()) {
    if (entity->body) {
      entity->body->IntegrateState(deltaTime);
    }
  }
}

void PhysicsEngine::ProcessCollisions(Scene& scene, float deltaTime) {
  auto& entities = scene.GetEntities();
  size_t count = entities.size();

  for (size_t i = 0; i < count; ++i) {
    for (size_t j = i + 1; j < count; ++j) {
      auto* entA = entities[i].get();
      auto* entB = entities[j].get();

      if (!entA->body || !entB->body) {
        continue;
      }

      if (entA->body->isStatic && entB->body->isStatic) {
        continue;
      }

      CollisionManifold manifold = entA->object->ComputeCollision(
          entB->object.get(), entA->body.get(), entB->body.get());

      if (manifold.isColliding) {
        ResolveCollision(entA->body.get(), entB->body.get(), manifold);
      }
    }
  }
}
