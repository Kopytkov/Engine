#pragma once

#include <optional>
#include <string>
#include "camera_parser.h"
#include "light_parser.h"
#include "object_parser.h"
#include "scene.h"

class SceneLoader {
 public:
  // Загружает сцену с поддержкой ref
  static SceneLoader Load(const std::string& jsonPath);

  // Получение данных
  Scene& GetScene() & { return scene_; }
  const Scene& GetScene() const& { return scene_; }
  Scene&& GetScene() && { return std::move(scene_); }
  std::optional<Camera> GetCamera() const { return camera_; }

 private:
  SceneLoader(Scene&& scene, std::optional<Camera> camera)
      : scene_(std::move(scene)), camera_(std::move(camera)) {}

  Scene scene_;
  std::optional<Camera> camera_;
};
