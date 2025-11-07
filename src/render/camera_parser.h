#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include "camera.h"

class CameraParser {
 public:
  static std::optional<Camera> Parse(const nlohmann::json& j);
};
