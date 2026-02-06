#pragma once
#include <memory>
#include "gl/shader.h"
#include "material.h"
#include "math/vec.h"

// Предварительные объявления для избежания циклических зависимостей
class Sphere;
class Box;
class PhysicsBody;

// Вспомогательная структура для данных о столкновении
struct CollisionManifold {
  bool isColliding = false;
  vec3 normal{0.0f};        // Нормаль столкновения
  float depth = 0.0f;       // Глубина проникновения
  vec3 contactPoint{0.0f};  // Точка контакта
};

class SceneObject {
 public:
  virtual ~SceneObject() = default;

  virtual float SDF(const vec3& point) const = 0;
  virtual vec3 getNormal(const vec3& point) const = 0;
  virtual const Material& GetMaterial() const = 0;
  virtual void UpdateUniforms(Shader& shader) const;
  virtual void SetRenderPosition(const vec3& position) = 0;
  virtual vec3 GetRenderPosition() const = 0;

  // Метод для вычисления столкновения с другим объектом
  virtual CollisionManifold ComputeCollision(
      const SceneObject* other,
      const PhysicsBody* myBody,
      const PhysicsBody* otherBody) const = 0;

  // Визиторы для двойной диспетчеризации
  virtual CollisionManifold ComputeCollisionWith(
      const Sphere* otherSphere,
      const PhysicsBody* myBody,
      const PhysicsBody* otherBody) const;

  virtual CollisionManifold ComputeCollisionWith(
      const Box* otherBox,
      const PhysicsBody* myBody,
      const PhysicsBody* otherBody) const;
};
