#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "control/camera_controller.h"
#include "gl/gl_renderer.h"
#include "gl/shader.h"
#include "input/input_controller.h"
#include "render/app_utils.h"
#include "render/camera.h"
#include "render/scene_loader.h"
#include "render/texture.h"

class Application {
 public:
  Application();
  ~Application();

  // Инициализация окна, контекста, сцены и ресурсов
  bool Initialize();

  // Главный цикл приложения
  void Run();

 private:
  // Внутренние методы цикла
  void Update(float deltaTime);
  void Render();
  void CalculateFPS(Uint32 currentTime);

 private:
  // Окно и Рендерер
  SDL_Window* window_;
  std::unique_ptr<GLRenderer> renderer_;

  // Состояние приложения
  bool running_;

  // Ввод и камера
  InputController input_;
  std::optional<Camera> camera_;
  std::optional<CameraController> cameraController_;

  // Сцена и ресурсы
  std::optional<SceneLoader> sceneLoader_;

  // Шейдеры и текстуры
  std::optional<Shader> raymarchShader_;
  std::vector<Texture> ballTextures_;
  std::vector<BallMaterialGPU> ballMaterialsGPU_;
  std::unique_ptr<Texture> dummyTexture_;

  // Данные для цикла и физики
  const int PHYSICS_UPDATES_PER_SECOND = 60;
  const float FIXED_DELTA_TIME = 1.0f / 60.0f;
  float timeAccumulator_ = 0.0f;

  // Данные FPS
  Uint32 lastTime_ = 0;
  Uint32 lastFPSTime_ = 0;
  int frameCount_ = 0;
  float currentFPS_ = 0.0f;
};
