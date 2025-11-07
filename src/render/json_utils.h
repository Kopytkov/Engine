#pragma once

#include <array>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;

inline std::array<float, 3> parseVec3(const json& j, const std::string& ctx) {
  if (!j.is_array() || j.size() < 3) {
    throw std::runtime_error("Invalid vec3 format: " + ctx);
  }
  return {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
}
