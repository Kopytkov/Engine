#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "json_utils.h"
#include "material_pbr.h"
#include "scene_entity.h"
#include "scene_object_box.h"
#include "scene_object_sphere.h"

class ObjectParser {
 public:
  static std::unique_ptr<SceneEntity> Parse(const nlohmann::json& j);
};
