#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include "camera.h"
#include "scene.h"

class SceneLoader {
 public:
  static std::pair<Scene, Camera> load(const std::string& jsonPath);

 private:
  static void validate(const nlohmann::json& j);
  static Camera parseCamera(const nlohmann::json& j);
  static std::unique_ptr<LightSource> parseLight(const nlohmann::json& j);
  static std::unique_ptr<SceneObject> parseObject(const nlohmann::json& j);
};
