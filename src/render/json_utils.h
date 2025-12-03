#pragma once
#include <array>
#include <cmath>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include "material_pbr.h"
#include "material_phong.h"

using json = nlohmann::json;

// Парсит vec3
inline std::array<float, 3> parseVec3(const json& j, const std::string& ctx) {
  if (!j.is_array() || j.size() < 3) {
    throw std::runtime_error("Invalid vec3 format: " + ctx);
  }
  return {j[0].get<float>(), j[1].get<float>(), j[2].get<float>()};
}

// Парсит material
inline std::unique_ptr<Material> parseMaterial(const json& j) {
  RGB color{255, 255, 255};
  std::shared_ptr<RawImage> albedoImage = nullptr;

  if (j.contains("color")) {
    auto c = j["color"];
    if (!c.is_array() || c.size() < 3) {
      throw std::runtime_error("Invalid material.color format");
    }
    color = RGB{static_cast<uint8_t>(c[0].get<float>() * 255),
                static_cast<uint8_t>(c[1].get<float>() * 255),
                static_cast<uint8_t>(c[2].get<float>() * 255)};
  }

  if (j.contains("texture")) {
    std::string path = j["texture"];
    albedoImage = std::make_shared<RawImage>(loadFromBMP(path));
  }

  std::string mat_type = j.value("type", "pbr");
  if (mat_type == "pbr") {
    float roughness = std::clamp(j.value("roughness", 0.18f), 0.03f, 1.0f);
    float metallic = std::clamp(j.value("metallic", 0.0f), 0.0f, 1.0f);
    float transmission = std::clamp(j.value("transparency", 0.0f), 0.0f, 1.0f);
    float refraction = j.value("refraction", 1.5f);
    return std::make_unique<MaterialPBR>(color, roughness, metallic,
                                         transmission, refraction, albedoImage);

  } else if (mat_type == "phong") {
    float roughness = std::clamp(j.value("roughness", 0.1f), 0.0f, 1.0f);
    return std::make_unique<MaterialPhong>(color, roughness);

  } else {
    throw std::runtime_error("Unknown material type: " + mat_type);
  }
}
