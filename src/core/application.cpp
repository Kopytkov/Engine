#include "application.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include "physics/physics_body.h"
#include "render/material_converter.h"
#include "render/raw_image.h"
#include "render/texture_manager.h"

Application::Application() : window_(nullptr), running_(false) {}

Application::~Application() {
  if (window_) {
    SDL_DestroyWindow(window_);
  }
  SDL_Quit();
}

bool Application::Initialize() {
  // Инициализация SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    return false;
  }

  // Настройка OpenGL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Создание окна 800x600
  window_ =
      SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
  if (!window_) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    return false;
  }

  // Инициализация GLRenderer
  renderer_ = std::make_unique<GLRenderer>(window_);
  if (!renderer_->Initialize()) {
    return false;
  }

  // Загрузка шейдеров для raymarching-рендеринга
  raymarchShader_.emplace("assets/shaders/vertex.glsl",
                          "assets/shaders/raymarch_fragment.glsl");
  if (raymarchShader_->getID() == 0) {
    std::cerr << "Failed to compile/link raymarch shader" << std::endl;
    return false;
  }

  // Инициализация менеджера текстур и загрузка основной сцены из JSON
  TextureManager::GetInstance().Initialize();
  sceneLoader_.emplace(SceneLoader::Load("assets/scene/billiard.json"));

  // // Тест на проверку гравитации и коллизии со столом
  // auto& scene = sceneLoader_->GetScene();
  // const auto& entities = scene.GetEntities();
  // if (entities.size() >= 2) {
  //   auto& ball_1 = entities[0];
  //   auto& ball_2 = entities[3];
  //   if (ball_1->object) {
  //     // Поднимаем шар над столом (Z = 20.0)
  //     ball_1->body->SetPosition(vec3(0.0f, 0.0f, 20.0f));
  //     ball_1->body->SetVelocity(vec3(0.0f, 0.0f, 0.0f));
  //     ball_2->body->SetVelocity(vec3(0.0f, 10.0f, 0.0f));
  //     // Слегка толкнем его вбок, чтобы он покатился после падения
  //     ball_1->body->ApplyForce(vec3(0.0f, 200.0f, 0.0f));
  //   }
  // }

  // Получение данных камеры из сцены
  auto camera_opt = sceneLoader_->GetCamera();
  if (!camera_opt) {
    std::cerr << "No camera in scene!" << std::endl;
    return false;
  }

  // Сохраняем камеру в член класса
  camera_ = *camera_opt;

  // Инициализация контроллера ввода
  input_.Initialize();

  // Инициализируем контроллер камеры
  cameraController_.emplace(*camera_, input_);
  cameraController_->SetSpeed(3.0f, 90.0f);

  // Пустая 1x1 текстура — используется как заглушка
  RawImage dummy(1, 1);
  dummyTexture_ = std::make_unique<Texture>(dummy);
  dummyTexture_->createTexture();

  // Настройка материалов, текстур и uniform'ов для бильярдных шаров
  if (raymarchShader_) {
    MaterialConverter::InitShaderSamplers(*raymarchShader_);
  }

  // Данные для расчета FPS
  lastTime_ = SDL_GetTicks();
  lastFPSTime_ = lastTime_;
  running_ = true;

  return true;
}

void Application::Run() {
  while (running_) {
    Uint32 now = SDL_GetTicks();
    float frameTime = (now - lastTime_) / 1000.0f;
    lastTime_ = now;

    timeAccumulator_ += frameTime;

    CalculateFPS(now);
    Update(frameTime);
    Render();
  }
}

void Application::CalculateFPS(Uint32 now) {
  ++frameCount_;
  Uint32 elapsedSinceLastFPS = now - lastFPSTime_;

  if (elapsedSinceLastFPS >= 200) {  // каждые 0.2 секунды
    currentFPS_ = frameCount_ * 1000.0f / elapsedSinceLastFPS;
    frameCount_ = 0;
    lastFPSTime_ = now;

    std::ostringstream title;
    title << "Game Engine | FPS: " << std::fixed << std::setprecision(1)
          << currentFPS_;
    SDL_SetWindowTitle(window_, title.str().c_str());
  }
}

void Application::Update(float deltaTime) {
  // Обработка событий ввода
  input_.PollEvents();
  if (input_.ShouldClose()) {
    running_ = false;
  }

  // Обновление камеры
  if (cameraController_) {
    cameraController_->HandleInput(deltaTime);
  }

  // Цикл обновления физики
  while (timeAccumulator_ >= FIXED_DELTA_TIME) {
    if (sceneLoader_) {
      sceneLoader_->GetScene().UpdatePhysics(FIXED_DELTA_TIME);
    }
    timeAccumulator_ -= FIXED_DELTA_TIME;
  }
}

void Application::Render() {
  if (!raymarchShader_ || !sceneLoader_ || !camera_) {
    return;
  }

  // Обновление uniform'ов всех объектов
  renderer_->UpdateUniforms(sceneLoader_->GetScene(), *raymarchShader_);

  raymarchShader_->use();

  // Передача параметров камеры в шейдер каждый кадр
  const Camera& cam = *camera_;
  vec3 right = normalize(cross(cam.GetViewVec(), cam.GetUpVec()));
  raymarchShader_->setVec3("cameraPos", cam.GetPosition());
  raymarchShader_->setVec3("cameraView", cam.GetViewVec());
  raymarchShader_->setVec3("cameraUp", cam.GetUpVec());
  raymarchShader_->setVec3("cameraRight", right);
  raymarchShader_->setFloat("tanFovHalf",
                            std::tan(cam.GetFOV() * 3.14159265f / 360.0f));
  raymarchShader_->setVec2("resolution", 800.0f, 600.0f);

  // Настройка освещения (точечный свет + глобальное направленное)
  raymarchShader_->setVec3("pointLightPos", vec3(0.0f, 2.7f, 10.0f));
  raymarchShader_->setVec3("pointLightColor", vec3(1.0f, 0.94f, 0.78f));
  raymarchShader_->setFloat("pointLightBrightness", 2.0f);
  raymarchShader_->setVec3("globalLightDir", vec3(0.0f, -1.0f, -0.5f));
  raymarchShader_->setVec3("globalLightColor", vec3(1.0f, 1.0f, 1.0f));
  raymarchShader_->setFloat("globalLightBrightness", 0.3f);

  // Рендеринг сцены через полноэкранный квад и raymarch-шейдер
  renderer_->Render(*raymarchShader_, *dummyTexture_);
  SDL_GL_SwapWindow(window_);
}
