#pragma once

#include <array>
#include <cmath>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include "material.h"

using json = nlohmann::json;

// Парсит vec3
inline std::array<float, 3> parseVec3(const json& j, const std::string& ctx) {
  if (!j.is_array() || j.size() < 3) {
    throw std::runtime_error("Invalid vec3 format: " + ctx);
  }
  return {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
}

inline Material parseMaterial(const json& j) {
  Material mat;

  if (j.contains("color")) {
    auto c = j["color"];
    if (!c.is_array() || c.size() < 3) {
      throw std::runtime_error("Invalid material.color format");
    }
    mat.base_color = RGB{static_cast<uint8_t>(c[0].get<float>() * 255),
                         static_cast<uint8_t>(c[1].get<float>() * 255),
                         static_cast<uint8_t>(c[2].get<float>() * 255)};
  }

  if (j.contains("reflectivity")) {
    mat.reflectivity = j["reflectivity"].get<float>();
  }
  if (j.contains("transparency")) {
    mat.transparency = j["transparency"].get<float>();
  }
  if (j.contains("refraction")) {
    mat.refraction = j["refraction"].get<float>();
  }
  if (j.contains("roughness")) {
    mat.roughness = j["roughness"].get<float>();
  }
  if (j.contains("metallic")) {
    mat.metallic = j["metallic"].get<float>();
  } else {
    mat.metallic = 0.0f;  // default для шаров — диэлектрик
  }

  return mat;
}
