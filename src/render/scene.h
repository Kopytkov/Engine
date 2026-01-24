#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <cmath>
#include "light_source.h"
#include "math/vec.h"
#include "math/vec_functions.h"
#include "ray.h"
#include "scene_entity.h"
#include "scene_object.h"

struct Hit {
  vec3 position;
  vec3 normal;
  SceneObject* obj;
};

class PhysicsEngine;

class Scene {
 public:
  Scene();
  ~Scene();

  // Запретить копирование
  Scene(const Scene&) = delete;
  Scene& operator=(const Scene&) = delete;

  // Разрешить перемещение
  Scene(Scene&&);
  Scene& operator=(Scene&&);

  void AddEntity(std::unique_ptr<SceneEntity> entity);
  void AddLight(std::unique_ptr<LightSource> light);
  void UpdatePhysics(float deltaTime);
  const std::vector<std::unique_ptr<SceneEntity>>& GetEntities() const;
  const std::vector<std::unique_ptr<LightSource>>& GetLights() const;
  vec3 CastRay(const Ray& ray, int depth) const;

  std::optional<Hit> GetHit(const Ray& ray,
                            SceneObject* ignore = nullptr) const;
  std::optional<Hit> GetHit(const Ray& ray,
                            float distance,
                            SceneObject* ignore = nullptr) const;

 private:
  std::vector<std::unique_ptr<SceneEntity>> entities_;
  std::vector<std::unique_ptr<LightSource>> lights_;
  std::unique_ptr<PhysicsEngine> physics_engine_;

  std::tuple<float, SceneObject*> GetDistance(
      const vec3& position,
      SceneObject* ignore = nullptr) const;
};
