#pragma once

#include "render/scene.h"

class PhysicsEngine {
 public:
  vec3 gravity{0.0f, 0.0f, -9.81f};  // Гравитация вниз по Z

  PhysicsEngine() = default;

  // Главный метод обновления физики
  void Update(Scene& scene, float deltaTime);

 private:
  // Применение гравитации ко всем объектам
  void ApplyGravity(Scene& scene);

  // Проверка и разрешение коллизий
  void ProcessCollisions(Scene& scene, float deltaTime);

  // Интеграция движения (позиция += скорость * время)
  void IntegrateBodies(Scene& scene, float deltaTime);
};
