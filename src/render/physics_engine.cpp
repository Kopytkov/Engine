#include "physics_engine.h"
#include "scene.h"
#include "scene_object_sphere.h"

void PhysicsEngine::ProcessCollisions(Scene& scene, float deltaTime) {
  auto& objects = scene.GetObjects();

  // Столкновения Шар-Шар
  for (size_t i = 0; i < objects.size(); ++i) {
    for (size_t j = i + 1; j < objects.size(); ++j) {
      // Определяем, является ли пара объектов двумя сферами
      auto* sphereA = dynamic_cast<Sphere*>(objects[i].get());
      auto* sphereB = dynamic_cast<Sphere*>(objects[j].get());

      // Если оба объекта - сферы, проверяем коллизию
      if (sphereA && sphereB) {
        vec3 posA = sphereA->GetPosition();
        vec3 posB = sphereB->GetPosition();
        float radiusA = sphereA->GetRadius();
        float radiusB = sphereB->GetRadius();

        vec3 delta = posA - posB;
        float distance = length(delta);

        // Фаза 1: Обнаружение столкновения
        if (distance < radiusA + radiusB) {
          // Фаза 2: Разрешение столкновения

          // Шаг 2.1: Разрешение проникновения (Positional Correction)
          // Выталкиваем шары друг из друга, чтобы они не застревали
          float overlap = (radiusA + radiusB) - distance;
          if (distance > kMinDistance) {
            vec3 correction = normalize(delta) * overlap;
            sphereA->SetPosition(posA + correction / 2.0f);
            sphereB->SetPosition(posB - correction / 2.0f);
          }

          // Шаг 2.2: Разрешение импульса (Impulse Resolution)
          // Модель упругого удара для тел равной массы
          // Расчет основан на 1D-формуле вдоль вектора нормали столкновения
          vec3 vA = sphereA->GetVelocity();
          vec3 vB = sphereB->GetVelocity();

          // Вектор нормали (линия, соединяющая центры)
          vec3 collisionNormal =
              (distance > 1e-6) ? normalize(delta) : vec3(1.0, 0.0, 0.0);

          // Проекции скоростей на нормаль
          float vA_proj = dot(vA, collisionNormal);
          float vB_proj = dot(vB, collisionNormal);

          // Новые скорости вдоль нормали - это проекции от другого шара
          float new_vA_proj = vB_proj;
          float new_vB_proj = vA_proj;

          // Вычисляем изменение скорости вдоль нормали
          vec3 delta_vA = (new_vA_proj - vA_proj) * collisionNormal;
          vec3 delta_vB = (new_vB_proj - vB_proj) * collisionNormal;

          // Устанавливаем новые скорости
          sphereA->SetVelocity(vA + delta_vA);
          sphereB->SetVelocity(vB + delta_vB);
        }
      }
    }
  }

  const auto& bounds = scene.GetTableBounds();

  // Столкновения Шар-Стол
  for (const auto& obj : objects) {
    if (auto* sphere = dynamic_cast<Sphere*>(obj.get())) {
      vec3 pos = sphere->GetPosition();
      vec3 vel = sphere->GetVelocity();
      float radius = sphere->GetRadius();
      float damping = 0.8f;  // Коэффициент затухания при столкновении со столом

      // Проверка по оси X
      if (pos[0] - radius < bounds.min[0]) {
        pos[0] = bounds.min[0] + radius;
        vel[0] = -vel[0] * damping;
      } else if (pos[0] + radius > bounds.max[0]) {
        pos[0] = bounds.max[0] - radius;
        vel[0] = -vel[0] * damping;
      }

      // Проверка по оси Y
      if (pos[1] - radius < bounds.min[1]) {
        pos[1] = bounds.min[1] + radius;
        vel[1] = -vel[1] * damping;
      } else if (pos[1] + radius > bounds.max[1]) {
        pos[1] = bounds.max[1] - radius;
        vel[1] = -vel[1] * damping;
      }

      // Применяем обновленные позицию и скорость
      sphere->SetPosition(pos);
      sphere->SetVelocity(vel);
    }
  }
}
