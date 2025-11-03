#include "light_parser.h"
#include "RGB.h"
#include "json_utils.h"
#include "light_source_global.h"
#include "light_source_point.h"

using json = nlohmann::json;

std::unique_ptr<LightSource> LightParser::Parse(const json& j) {
  if (!j.contains("type")) {
    throw std::runtime_error("Missing light.type");
  }

  std::string type = j["type"].get<std::string>();
  float brightness = j.value("brightness", 1.0f);

  std::array<float, 3> col_arr = {1, 1, 1};
  if (j.contains("color")) {
    col_arr = parseVec3(j["color"], "light.color");
  }

  RGB col = {static_cast<uint8_t>(col_arr[0] * 255),
             static_cast<uint8_t>(col_arr[1] * 255),
             static_cast<uint8_t>(col_arr[2] * 255)};

  if (type == "point") {
    if (!j.contains("position")) {
      throw std::runtime_error("Missing light.position");
    }
    auto pos_arr = parseVec3(j["position"], "light.position");
    vec3 pos(pos_arr[0], pos_arr[1], pos_arr[2]);
    return std::make_unique<PointLightSource>(pos, brightness, col);
  }
  if (type == "global") {
    if (!j.contains("direction")) {
      throw std::runtime_error("Missing light.direction");
    }
    auto dir_arr = parseVec3(j["direction"], "light.direction");
    vec3 dir(dir_arr[0], dir_arr[1], dir_arr[2]);
    return std::make_unique<GlobalLight>(dir, brightness, col);
  }

  throw std::runtime_error("Unknown light type: " + type);
}
