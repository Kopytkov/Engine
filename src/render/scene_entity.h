#pragma once

#include <memory>
#include "physics/physics_body.h"
#include "scene_object.h"

struct SceneEntity {
  std::unique_ptr<SceneObject> object;  // Графическое представление
  std::unique_ptr<PhysicsBody> body;    // Физическая модель

  SceneEntity(std::unique_ptr<SceneObject> obj, std::unique_ptr<PhysicsBody> bd)
      : object(std::move(obj)), body(std::move(bd)) {}
};
