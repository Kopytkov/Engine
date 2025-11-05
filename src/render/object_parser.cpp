#include "object_parser.h"
#include "json_utils.h"
#include "material.h"
#include "scene_object_sphere.h"

using json = nlohmann::json;

std::unique_ptr<SceneObject> ObjectParser::Parse(const json& j) {
  if (!j.contains("type")) {
    throw std::runtime_error("Missing object.type");
  }
  if (!j.contains("position")) {
    throw std::runtime_error("Missing object.position");
  }

  std::string type = j["type"].get<std::string>();
  auto pos_arr = parseVec3(j["position"], "object.position");
  vec3 pos(pos_arr[0], pos_arr[1], pos_arr[2]);

  if (type == "sphere") {
    if (!j.contains("radius")) {
      throw std::runtime_error("Missing object.radius");
    }
    float r = j["radius"].get<float>();
    if (r <= 0) {
      throw std::runtime_error("Invalid radius");
    }

    // Парсим материал
    Material mat;
    if (j.contains("material")) {
      const auto& m = j["material"];
      if (m.contains("color")) {
        auto c = m["color"];
        mat.base_color = RGB{static_cast<uint8_t>(c[0].get<float>() * 255),
                             static_cast<uint8_t>(c[1].get<float>() * 255),
                             static_cast<uint8_t>(c[2].get<float>() * 255)};
      }
      if (m.contains("reflectivity")) {
        mat.reflectivity = m["reflectivity"];
      }
      if (m.contains("transparency")) {
        mat.transparency = m["transparency"];
      }
      if (m.contains("refraction")) {
        mat.refraction = m["refraction"];
      }
    }

    return std::make_unique<Sphere>(pos, r, mat);
  }

  throw std::runtime_error("Unknown object type: " + type);
}
