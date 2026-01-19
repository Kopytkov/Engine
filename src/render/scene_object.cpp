#include "scene_object.h"

void SceneObject::UpdateUniforms(Shader& shader) const {
  // Базовая реализация пока остается пустой
}

void SceneObject::IntegrateState(float deltaTime) {
  // Обновляем позицию на основе скорости
  SetPosition(GetPosition() + velocity_ * deltaTime);
}

void SceneObject::SetVelocity(const vec3& velocity) {
  velocity_ = velocity;
}

vec3 SceneObject::GetVelocity() const {
  return velocity_;
}
