#include "object_parser.h"
#include "json_utils.h"
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
    return std::make_unique<Sphere>(pos, r);
  }

  // ← Легко добавить:
  // if (type == "cube") return std::make_unique<Cube>(...);
  // if (type == "plane") return std::make_unique<Plane>(...);

  throw std::runtime_error("Unknown object type: " + type);
}
