#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include "camera.h"
#include "json_utils.h"
#include "math/vec_functions.h"

class CameraParser {
 public:
  static std::optional<Camera> Parse(const nlohmann::json& j);
};
