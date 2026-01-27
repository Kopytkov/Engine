#include "physics_engine.h"
#include <algorithm>
#include <cmath>
#include "math/vec_functions.h"
#include "render/scene_object_box.h"
#include "render/scene_object_sphere.h"

constexpr float kFrictionThreshold =
    1e-4f;  // Порог касательной скорости для трения
constexpr float kRestingVelocityThreshold =
    0.05f;  // Порог нормальной скорости для "залипания"
constexpr float kEpsilon =
    1e-6f;  // Минимальная дистанция для предотвращения деления на ноль

// Вспомогательная структура для данных о столкновении
struct CollisionManifold {
  bool isColliding = false;
  vec3 normal{0.0f};        // Нормаль столкновения
  float depth = 0.0f;       // Глубина проникновения
  vec3 contactPoint{0.0f};  // Точка контакта на поверхности
};

CollisionManifold DetectCollisionSphereSphere(const Sphere* a,
                                              const PhysicsBody* bodyA,
                                              const Sphere* b,
                                              const PhysicsBody* bodyB) {
  CollisionManifold result;
  vec3 posA = bodyA->GetPosition();
  vec3 posB = bodyB->GetPosition();
  vec3 delta = posA - posB;
  float dist = length(delta);
  float radiusSum = a->GetRadius() + b->GetRadius();

  if (dist < radiusSum) {
    result.isColliding = true;
    result.depth = radiusSum - dist;
    // Если центры совпадают, выбираем произвольную ось Y
    result.normal = (dist > 1e-6f) ? normalize(delta) : vec3(0, 1, 0);

    // Точка контакта лежит посередине между поверхностями (упрощенно)
    result.contactPoint = posA - result.normal * a->GetRadius();
  }
  return result;
}

CollisionManifold DetectCollisionSphereBox(const Sphere* sphere,
                                           const PhysicsBody* bodySphere,
                                           const Box* box,
                                           const PhysicsBody* bodyBox) {
  CollisionManifold result;

  vec3 spherePos = bodySphere->GetPosition();
  vec3 boxPos = bodyBox->GetPosition();

  // Получаем матрицу поворота коробки
  mat3<float> boxRot = quatToMat3(bodyBox->orientation);

  // Переводим сферу в локальную систему координат коробки
  vec3 relPos = spherePos - boxPos;
  vec3 localSpherePos = MulMatTransposedVec(boxRot, relPos);

  // Находим ближайшую точку на поверхности коробки к центру сферы
  vec3 boxHalfExtents = box->GetHalfExtents();
  vec3 closestPointLocal = vec3(
      std::clamp(localSpherePos[0], -boxHalfExtents[0], boxHalfExtents[0]),
      std::clamp(localSpherePos[1], -boxHalfExtents[1], boxHalfExtents[1]),
      std::clamp(localSpherePos[2], -boxHalfExtents[2], boxHalfExtents[2]));

  // Проверяем расстояние от ближайшей точки до центра сферы
  vec3 differenceLocal = localSpherePos - closestPointLocal;
  float distance = length(differenceLocal);
  float radius = sphere->GetRadius();

  if (distance < radius) {
    result.isColliding = true;
    result.depth = radius - distance;

    // Вычисляем нормаль в локальном пространстве
    vec3 normalLocal;
    if (distance < kEpsilon) {
      // Сфера внутри - берем вектор до центра
      normalLocal = normalize(localSpherePos);
    } else {
      normalLocal = normalize(differenceLocal);
    }

    // Переводим результат обратно в мировые координаты
    result.normal = MulMatVec(boxRot, normalLocal);
    result.contactPoint = boxPos + MulMatVec(boxRot, closestPointLocal);
  }

  return result;
}

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

      // Пропускаем, если у кого-то нет физики
      if (!entA->body || !entB->body) {
        continue;
      }

      // Пропускаем, если оба статичны
      if (entA->body->isStatic && entB->body->isStatic) {
        continue;
      }

      CollisionManifold manifold;
      auto* sphereA = dynamic_cast<Sphere*>(entA->object.get());
      auto* sphereB = dynamic_cast<Sphere*>(entB->object.get());
      auto* boxA = dynamic_cast<Box*>(entA->object.get());
      auto* boxB = dynamic_cast<Box*>(entB->object.get());

      // Sphere vs Sphere
      if (sphereA && sphereB) {
        manifold = DetectCollisionSphereSphere(sphereA, entA->body.get(),
                                               sphereB, entB->body.get());
      }
      // Sphere vs Box
      else if (sphereA && boxB) {
        manifold = DetectCollisionSphereBox(sphereA, entA->body.get(), boxB,
                                            entB->body.get());
      }
      // Box vs Sphere (зеркально)
      else if (boxA && sphereB) {
        manifold = DetectCollisionSphereBox(sphereB, entB->body.get(), boxA,
                                            entA->body.get());
        manifold.normal = -manifold.normal;  // Инвертируем нормаль
      }

      if (manifold.isColliding) {
        ResolveCollision(entA->body.get(), entB->body.get(), manifold);
      }
    }
  }
}
