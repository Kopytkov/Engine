#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "light_source.h"
#include "math/vec.h"
#include "ray.h"
#include "scene_object.h"

class SceneFactory {
  static std::unique_ptr<SceneObject> CreateSphere(const vec3& position,
                                                   float r);
};

struct Hit {
  vec3 position;
  vec3 normal;
  SceneObject* obj;
};

class Scene {
 public:
  Scene();
  ~Scene();

  void AddObject(std::unique_ptr<SceneObject> object);
  void AddLight(std::unique_ptr<LightSource> light);
  const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const;
  const std::vector<std::unique_ptr<LightSource>>& GetLights() const;

  std::optional<Hit> GetHit(const Ray& ray) const;

 private:
  std::vector<std::unique_ptr<SceneObject>> objects_;
  std::vector<std::unique_ptr<LightSource>> lights_;

  std::tuple<float, SceneObject*> GetDistance(const vec3& position) const;
};
