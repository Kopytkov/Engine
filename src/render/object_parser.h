#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "scene_object.h"

class ObjectParser {
 public:
  static std::unique_ptr<SceneObject> Parse(const nlohmann::json& j);
};
