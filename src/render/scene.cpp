#include "scene.h"
#include "physics_engine.h"

Scene::Scene() {
  physics_engine_ = std::make_unique<PhysicsEngine>();
}

Scene::~Scene() = default;

Scene::Scene(Scene&&) = default;
Scene& Scene::operator=(Scene&&) = default;

void Scene::AddObject(std::unique_ptr<SceneObject> object) {
  objects_.push_back(std::move(object));
}

void Scene::AddLight(std::unique_ptr<LightSource> light) {
  lights_.push_back(std::move(light));
}

void Scene::UpdatePhysics(float deltaTime) {
  // Обрабатываем столкновения
  if (physics_engine_) {
    physics_engine_->ProcessCollisions(*this, deltaTime);
  }

  // Обновляем позиции
  for (const auto& obj : objects_) {
    if (obj) {
      obj->IntegrateState(deltaTime);
    }
  }
}

const std::vector<std::unique_ptr<SceneObject>>& Scene::GetObjects() const {
  return objects_;
}

const std::vector<std::unique_ptr<LightSource>>& Scene::GetLights() const {
  return lights_;
}

vec3 Scene::CastRay(const Ray& ray, int depth) const {
  if (depth > 5) {
    return vec3(0.0f);  // Если лучи запутались, возвращаем черный
  }

  if (auto hit_opt = GetHit(ray); hit_opt) {
    const Hit& hit = *hit_opt;
    const Material& mat = hit.obj->GetMaterial();

    vec3 V = normalize(-ray.direction);
    return mat.shade(hit, *this, V, depth);
  }

  // Если луч улетел в пустоту, рисуем градиент
  vec3 unit_direction = normalize(ray.direction);

  // t меняется от 0 до 1
  float t = 0.5f * (unit_direction[1] + 1.0f);

  // Линейная интерполяция от белого к голубому
  return mix(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), t);
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
    float d = std::abs(obj->SDF(position));
    if (d < result) {
      result = d;
      nearest = obj.get();
    }
  }
  return {result, nearest};
}
