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

std::optional<Hit> Scene::GetHit(const Ray& ray, float distance) const {
  vec3 position = ray.position;
  float traveled = 0.0f;
  for (uint32_t hop = ray.numOfStep; hop < kMaxStep and traveled < distance; ++hop) {
    const auto [d, obj] = GetDistance(position);
    if (d < kMinDistance) {
      Hit hit;
      hit.position = position;
      hit.obj = obj;
      hit.normal = obj->getNormal(position);

      return hit;
    }
    position = position + ray.direction * d;
    traveled += d;
  }
  return std::nullopt;
}

std::optional<Hit> Scene::GetHit(const Ray& ray) const {
  return GetHit(ray, kMaxDistance);
}

std::tuple<float, SceneObject*> Scene::GetDistance(const vec3& position) const {
  float result = kMaxDistance;
  SceneObject* nearestObj;
  for (const auto& object : objects_) {
    const float d = object->SDF(position);
    if (d < result) {
      result = d;
      nearestObj = object.get();
    }
  }
  return {result, nearestObj};
}
