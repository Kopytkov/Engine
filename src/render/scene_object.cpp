#include "scene_object.h"

void SceneObject::UpdateUniforms(Shader& shader) const {
  // Базовая реализация пока остается пустой
}

CollisionManifold SceneObject::ComputeCollisionWith(
    const Sphere* otherSphere,
    const PhysicsBody* myBody,
    const PhysicsBody* otherBody) const {
  return {};  // По умолчанию нет коллизии
}

CollisionManifold SceneObject::ComputeCollisionWith(
    const Box* otherBox,
    const PhysicsBody* myBody,
    const PhysicsBody* otherBody) const {
  return {};  // По умолчанию нет коллизии
}
