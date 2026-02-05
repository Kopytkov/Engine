#include <SDL2/SDL.h>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "gl/gl_renderer.h"
#include "gl/shader.h"
#include "render/app_utils.h"
#include "render/camera.h"
#include "render/raw_image.h"
#include "render/scene_entity.h"
#include "render/scene_loader.h"
#include "render/texture.h"
#include "render/texture_manager.h"

int main(int argc, char* argv[]) {
  // Инициализация SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    return -1;
  }

  // Настройка OpenGL
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Создание окна 800x600
  SDL_Window* window =
      SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
  if (!window) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  // Инициализация GLRenderer
  GLRenderer glRenderer(window);
  if (!glRenderer.Initialize()) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  // Загрузка шейдеров для raymarching-рендеринга
  Shader raymarchShader("assets/shaders/vertex.glsl",
                        "assets/shaders/raymarch_fragment.glsl");
  if (raymarchShader.getID() == 0) {
    std::cerr << "Failed to compile/link raymarch shader" << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  // Инициализация менеджера текстур и загрузка основной сцены из JSON
  TextureManager::GetInstance().Initialize();
  auto sceneLoader = SceneLoader::Load("assets/scene/billiard.json");
  auto& scene = sceneLoader.GetScene();

  // const auto& entities = scene.GetEntities();

  // // Тест на проверку гравитации и коллизии со столом
  // if (entities.size() >= 2) {
  //   auto& ball_1 = entities[0];

  //   if (ball_1->body) {
  //     // Поднимаем шар над столом (Z = 20.0)
  //     ball_1->body->SetPosition(vec3(0.0f, 0.0f, 20.0f));
  //     ball_1->body->SetVelocity(vec3(0.0f, 0.0f, 0.0f));

  //     // Слегка толкнем его вбок, чтобы он покатился после падения
  //     ball_1->body->ApplyForce(vec3(0.0f, 200.0f, 0.0f));
  //   }
  // }

  // Получение данных камеры из сцены
  auto camera_opt = sceneLoader.GetCamera();
  if (!camera_opt) {
    std::cerr << "No camera in scene!" << std::endl;
    return -1;
  }
  Camera camera = *camera_opt;

  // Пустая 1x1 текстура — используется как заглушка
  RawImage dummy(1, 1);
  Texture dummyTexture(dummy);
  dummyTexture.createTexture();

  // Настройка материалов, текстур и uniform'ов для бильярдных шаров
  std::vector<Texture> g_ballTextures;
  std::vector<BallMaterialGPU> g_ballMaterialsGPU;

  if (!AppUtils::InitRaymarchBallResources(
          scene, raymarchShader, g_ballTextures, g_ballMaterialsGPU)) {
    std::cerr << "Failed to init raymarch ball resources\n";
    return -1;
  }

  // Привязываем текстуры шаров
  AppUtils::BindBallTextures(g_ballTextures, raymarchShader);

  // Основной цикл приложения
  bool running = true;

  // Логика фиксированного шага времени
  float timeAccumulator = 0.0f;  // Накапливает время кадра
  const int PHYSICS_UPDATES_PER_SECOND = 60;
  const float fixedDeltaTime = 1.0f / PHYSICS_UPDATES_PER_SECOND;

  // Переменные для расчета FPS
  Uint32 lastTime = SDL_GetTicks();
  Uint32 lastFPSTime = lastTime;
  int frameCount = 0;
  float smoothFPS = 60.0f;
  float currentFPS = 0.0f;

  const float moveSpeed = 3.0f;
  const float rotateSpeed = 90.0f;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  while (running) {
    Uint32 now = SDL_GetTicks();
    float frameTime = (now - lastTime) / 1000.0f;
    lastTime = now;
    timeAccumulator += frameTime;

    // Расчет FPS
    ++frameCount;
    Uint32 elapsedSinceLastFPS = now - lastFPSTime;
    if (elapsedSinceLastFPS >= 200) {  // каждые 0.2 секунды
      currentFPS = frameCount * 1000.0f / elapsedSinceLastFPS;
      frameCount = 0;
      lastFPSTime = now;

      std::ostringstream title;
      title << "Game Engine | FPS: " << std::fixed << std::setprecision(1)
            << currentFPS;
      SDL_SetWindowTitle(window, title.str().c_str());
    }

    // Обработка событий
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    // Обработка клавиатурного ввода
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_W]) {
      camera.Move(camera.GetUpVec() * moveSpeed * frameTime);
    }
    if (keys[SDL_SCANCODE_S]) {
      camera.Move(-camera.GetUpVec() * moveSpeed * frameTime);
    }
    if (keys[SDL_SCANCODE_A]) {
      vec3 left = normalize(cross(camera.GetUpVec(), camera.GetViewVec()));
      camera.Move(left * moveSpeed * frameTime);
    }
    if (keys[SDL_SCANCODE_D]) {
      vec3 right = normalize(cross(camera.GetViewVec(), camera.GetUpVec()));
      camera.Move(right * moveSpeed * frameTime);
    }
    if (keys[SDL_SCANCODE_SPACE]) {
      camera.Move(camera.GetViewVec() * moveSpeed * frameTime);
    }
    if (keys[SDL_SCANCODE_LCTRL]) {
      camera.Move(-camera.GetViewVec() * moveSpeed * frameTime);
    }
    if (keys[SDL_SCANCODE_ESCAPE]) {
      running = false;
    }

    // Поворот камеры по движению мыши
    int mx, my;
    if (SDL_GetRelativeMouseState(&mx, &my) && (mx || my)) {
      camera.Rotate(static_cast<float>(mx) * rotateSpeed * frameTime,
                    -static_cast<float>(my) * rotateSpeed * frameTime);
    }

    // Цикл обновления физики
    while (timeAccumulator >= fixedDeltaTime) {
      sceneLoader.GetScene().UpdatePhysics(fixedDeltaTime);
      timeAccumulator -= fixedDeltaTime;
    }

    // Обновление uniform'ов всех объектов
    glRenderer.UpdateUniforms(sceneLoader.GetScene(), raymarchShader);

    // Передача параметров камеры в шейдер каждый кадр
    raymarchShader.use();

    vec3 right = normalize(cross(camera.GetViewVec(), camera.GetUpVec()));
    raymarchShader.setVec3("cameraPos", camera.GetPosition());
    raymarchShader.setVec3("cameraView", camera.GetViewVec());
    raymarchShader.setVec3("cameraUp", camera.GetUpVec());
    raymarchShader.setVec3("cameraRight", right);
    raymarchShader.setFloat("tanFovHalf",
                            std::tan(camera.GetFOV() * 3.14159265f / 360.0f));
    raymarchShader.setVec2("resolution", 800.0f, 600.0f);

    // Настройка освещения (точечный свет + глобальное направленное)
    raymarchShader.setVec3("pointLightPos", vec3(0.0f, 2.7f, 10.0f));
    raymarchShader.setVec3("pointLightColor", vec3(1.0f, 0.94f, 0.78f));
    raymarchShader.setFloat("pointLightBrightness", 2.0f);
    raymarchShader.setVec3("globalLightDir", vec3(0.0f, -1.0f, -0.5f));
    raymarchShader.setVec3("globalLightColor", vec3(1.0f, 1.0f, 1.0f));
    raymarchShader.setFloat("globalLightBrightness", 0.3f);

    // Рендеринг сцены через полноэкранный квад и raymarch-шейдер
    glRenderer.Render(raymarchShader, dummyTexture);
    SDL_GL_SwapWindow(window);
  }

  // Очистка ресурсов
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
