#include "object_parser.h"

using json = nlohmann::json;

std::unique_ptr<SceneObject> ObjectParser::Parse(const json& j) {
  if (!j.contains("type")) {
    throw std::runtime_error("Missing object.type");
  }
  if (!j.contains("position")) {
    throw std::runtime_error("Missing object.position");
  }

  std::string type = j["type"].get<std::string>();
  vec3 pos = vec3(parseVec3(j["position"], "object.position"));

  // Парсим sphere
  if (type == "sphere") {
    if (!j.contains("radius")) {
      throw std::runtime_error("Missing object.radius");
    }
    float r = j["radius"].get<float>();
    if (r <= 0) {
      throw std::runtime_error("Invalid radius");
    }

    auto mat_ptr = j.contains("material")
                       ? parseMaterial(j["material"])
                       : parseMaterial(json{{"type", "pbr"}});

    return std::make_unique<Sphere>(pos, r, std::move(mat_ptr));
  }

  // Парсим box
  if (type == "box") {
    if (!j.contains("vertex")) {
      throw std::runtime_error("Missing object.vertex for box");
    }

    vec3 vertex = vec3(parseVec3(j["vertex"], "object.vertex"));

    auto mat_ptr = j.contains("material")
                       ? parseMaterial(j["material"])
                       : parseMaterial(json{{"type", "pbr"}});

    return std::make_unique<Box>(pos, vertex, std::move(mat_ptr));
  }

  throw std::runtime_error("Unknown object type: " + type);
}
