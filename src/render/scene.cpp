#include "scene.h"

void Scene::AddObject(std::unique_ptr<SceneObject> object) {
  objects_.push_back(std::move(object));
}

void Scene::AddLight(std::unique_ptr<LightSource> light) {
  lights_.push_back(std::move(light));
}

const std::vector<std::unique_ptr<SceneObject>>& Scene::GetObjects() const {
  return objects_;
}

const std::vector<std::unique_ptr<LightSource>>& Scene::GetLights() const {
  return lights_;
}

std::optional<Hit> Scene::GetHit(const Ray& ray,
                                 float max_distance,
                                 SceneObject* ignore) const {
  vec3 position = ray.position;
  float traveled = 0.0f;
  uint32_t step = ray.numOfStep;

  while (step < kMaxStep && traveled < max_distance) {
    auto [d, obj] = GetDistance(position, ignore);
    if (d < kMinDistance) {
      Hit hit;
      hit.position = position;
      hit.obj = obj;
      hit.normal = obj->getNormal(position);
      return hit;
    }
    position = position + ray.direction * d;
    traveled += d;
    ++step;
  }
  return std::nullopt;
}

std::optional<Hit> Scene::GetHit(const Ray& ray, SceneObject* ignore) const {
  return GetHit(ray, kMaxDistance, ignore);
}

std::tuple<float, SceneObject*> Scene::GetDistance(const vec3& position,
                                                   SceneObject* ignore) const {
  float result = kMaxDistance;
  SceneObject* nearest = nullptr;
  for (const auto& obj : objects_) {
    if (obj.get() == ignore) {
      continue;
    }
    float d = obj->SDF(position);
    if (d < result) {
      result = d;
      nearest = obj.get();
    }
  }
  return {result, nearest};
}
