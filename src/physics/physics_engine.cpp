#include "physics_engine.h"
#include <algorithm>
#include "math/vec_functions.h"
#include "render/scene_object_box.h"
#include "render/scene_object_sphere.h"

// Вспомогательная структура для данных о столкновении
struct CollisionManifold {
  bool isColliding = false;
  vec3 normal{0.0f};   // Нормаль столкновения
  float depth = 0.0f;  // Глубина проникновения
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

  // Переводим сферу в локальную систему координат коробки (пока без вращения)
  vec3 localSpherePos = spherePos - boxPos;

  // Находим ближайшую точку на поверхности коробки к центру сферы
  vec3 boxHalfExtents = box->GetHalfExtents();
  vec3 closestPoint = vec3(
      std::clamp(localSpherePos[0], -boxHalfExtents[0], boxHalfExtents[0]),
      std::clamp(localSpherePos[1], -boxHalfExtents[1], boxHalfExtents[1]),
      std::clamp(localSpherePos[2], -boxHalfExtents[2], boxHalfExtents[2]));

  // Проверяем расстояние от ближайшей точки до центра сферы
  vec3 difference = localSpherePos - closestPoint;
  float distance = length(difference);
  float radius = sphere->GetRadius();

  if (distance < radius) {
    result.isColliding = true;
    result.depth = radius - distance;

    // Если сфера внутри коробки
    if (distance < 1e-6f) {
      // Выталкиваем наружу (упрощенно по радиус-вектору)
      result.normal = normalize(localSpherePos);
    } else {
      result.normal = normalize(difference);
    }
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
  float invMassA =
      (bodyA->isStatic || bodyA->mass == 0) ? 0.0f : 1.0f / bodyA->mass;
  float invMassB =
      (bodyB->isStatic || bodyB->mass == 0) ? 0.0f : 1.0f / bodyB->mass;
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
  vec3 rv =
      bodyA->GetVelocity() - bodyB->GetVelocity();  // Относительная скорость
  float velAlongNormal = dot(rv, manifold.normal);

  // Если объекты удаляются друг от друга, не трогаем их
  if (velAlongNormal > 0) {
    return;
  }

  // Вычисляем упругость (берем минимальную из двух)
  float e =
      std::min(bodyA->material->restitution, bodyB->material->restitution);

  // Скаляр импульса
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

  // TODO: Реализовать модель Кулонова трения (Static & Dynamic Friction).
  // Это позволит симулировать скольжение и вращающий момент (Torque).
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
      if ((entA->body->isStatic || entA->body->mass == 0) &&
          (entB->body->isStatic || entB->body->mass == 0)) {
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
