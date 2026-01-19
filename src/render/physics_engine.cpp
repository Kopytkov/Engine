#include "physics_engine.h"
#include "scene.h"
#include "scene_object_sphere.h"

void PhysicsEngine::ProcessCollisions(Scene& scene, float deltaTime) {
  auto& objects = scene.GetObjects();

  // Проверяем каждую пару объектов на столкновение
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
}
