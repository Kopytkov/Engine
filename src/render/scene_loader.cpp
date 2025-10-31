#include "scene_loader.h"
#include <array>
#include <fstream>
#include <nlohmann/json.hpp>
#include "light_source_global.h"
#include "light_source_point.h"
#include "math/vec_functions.h"
#include "scene_object_sphere.h"

using json = nlohmann::json;

std::pair<Scene, Camera> SceneLoader::load(const std::string& jsonPath) {
  std::ifstream file(jsonPath);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open JSON: " + jsonPath);
  }

  json j;
  file >> j;
  validate(j);

  Scene scene;
  Camera camera = parseCamera(j["camera"]);
  std::string basePath = std::filesystem::path(jsonPath).parent_path().string();

  // === Lights ===
  if (j.contains("lights")) {
    for (const auto& l : j["lights"]) {
      scene.AddLight(parseLight(l));
    }
  }

  // === Objects ===
  if (j.contains("objects")) {
    for (const auto& o : j["objects"]) {
      scene.AddObject(parseObject(o));
    }
  }

  // Перемещение Scene
  return std::make_pair(std::move(scene), std::move(camera));
}

void SceneLoader::validate(const json& j) {
  // Проверяем только наличие корневых полей
  if (!j.contains("camera")) {
    throw std::runtime_error("Missing: camera");
  }
  // Остальное проверяется внутри parseCamera / parseObject
}

Camera SceneLoader::parseCamera(const json& j) {
  // === Проверка наличия обязательных полей ===
  const std::vector<std::string> required = {"position", "target", "resolution",
                                             "fov_deg"};
  for (const auto& field : required) {
    if (!j.contains(field)) {
      throw std::runtime_error("Missing: camera." + field);
    }
  }

  // === Чтение позиции и цели ===
  vec3 pos(j["position"][0].get<float>(), j["position"][1].get<float>(),
           j["position"][2].get<float>());
  vec3 target(j["target"][0].get<float>(), j["target"][1].get<float>(),
              j["target"][2].get<float>());

  vec3 view = normalize(target - pos);

  // === Up-вектор (по умолчанию: Y вверх) ===
  std::vector<float> up_vec =
      j.value("up", std::vector<float>{0.0f, 1.0f, 0.0f});
  if (up_vec.size() < 3) {
    throw std::runtime_error("Invalid: camera.up — must have 3 components");
  }
  vec3 up(up_vec[0], up_vec[1], up_vec[2]);

  // === Разрешение ===
  auto res = j["resolution"].get<std::array<int, 2>>();
  int w = res[0], h = res[1];
  if (w <= 0 || h <= 0) {
    throw std::runtime_error("Invalid: camera.resolution — must be positive");
  }

  float pix = j.value("pixel_size_mm", 0.26f);
  float fov = j["fov_deg"].get<float>();

  return Camera(pos, view, up, w, h, pix, fov);
}

std::unique_ptr<LightSource> SceneLoader::parseLight(const json& j) {
  if (!j.contains("type")) {
    throw std::runtime_error("Missing: light.type");
  }

  std::string type = j["type"].get<std::string>();
  float br = j.value("brightness", 1.0f);

  auto col_vec = j.value("color", std::vector<float>{1.0f, 1.0f, 1.0f});
  if (col_vec.size() < 3) {
    throw std::runtime_error("Invalid: light.color — must have 3 components");
  }
  RGB col = {static_cast<uint8_t>(col_vec[0] * 255),
             static_cast<uint8_t>(col_vec[1] * 255),
             static_cast<uint8_t>(col_vec[2] * 255)};

  if (type == "point") {
    if (!j.contains("position")) {
      throw std::runtime_error("Missing: light.position for point light");
    }
    vec3 pos(j["position"][0].get<float>(), j["position"][1].get<float>(),
             j["position"][2].get<float>());
    return std::make_unique<PointLightSource>(pos, br, col);
  } else if (type == "global") {
    if (!j.contains("direction")) {
      throw std::runtime_error("Missing: light.direction for global light");
    }
    vec3 dir(j["direction"][0].get<float>(), j["direction"][1].get<float>(),
             j["direction"][2].get<float>());
    return std::make_unique<GlobalLight>(dir, br, col);
  } else {
    throw std::runtime_error("Unknown light type: " + type);
  }
}

std::unique_ptr<SceneObject> SceneLoader::parseObject(const json& j) {
  if (!j.contains("type")) {
    throw std::runtime_error("Missing: object.type");
  }
  if (!j.contains("position")) {
    throw std::runtime_error("Missing: object.position");
  }

  std::string type = j["type"].get<std::string>();
  vec3 pos(j["position"][0].get<float>(), j["position"][1].get<float>(),
           j["position"][2].get<float>());

  if (type == "sphere") {
    if (!j.contains("radius")) {
      throw std::runtime_error("Missing: object.radius");
    }
    float r = j["radius"].get<float>();
    if (r <= 0.0f) {
      throw std::runtime_error("Invalid: object.radius");
    }
    return std::make_unique<Sphere>(pos, r);
  } else {
    throw std::runtime_error("Unknown object type: " + type);
  }
}
