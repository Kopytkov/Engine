#pragma once

class Scene;

class PhysicsEngine {
 public:
  PhysicsEngine() = default;

  // Проверка и обработка всех столкновений в сцене
  void ProcessCollisions(Scene& scene, float deltaTime);
};
