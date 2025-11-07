#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "light_source.h"

class LightParser {
 public:
  static std::unique_ptr<LightSource> Parse(const nlohmann::json& j);
};
