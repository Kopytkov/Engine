#include <SDL2/SDL.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "bmp/bmp.h"
#include "gl/gl_renderer.h"
#include "gl/shader.h"
#include "render/raw_image.h"
#include "render/renderer.h"
#include "render/scene_loader.h"
#include "render/texture.h"
#include "render/texture_manager.h"

using json = nlohmann::json;

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

  // Загрузка шейдеров
  Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

  // Проверка и генерация текстур
  try {
    TextureManager::GetInstance().Initialize();
  } catch (const std::exception& e) {
    std::cerr << "[Textures] Error during initialization: " << e.what() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  // === ЗАГРУЗКА СЦЕНЫ ИЗ JSON ===
  try {
    auto loader = SceneLoader::Load("assets/scene/billiard.json");
    Scene scene = std::move(loader).GetScene();

    RawImage image(800, 600);
    Renderer renderer;

    // Камера — проверяем optional
    auto camera_opt = loader.GetCamera();
    if (!camera_opt) {
      throw std::runtime_error(
          "Camera is missing in JSON, but required for rendering.");
    }

    renderer.Render(*camera_opt, scene, image);

    // Сохранение изображения в BMP файл
    BMP bmp(image);
    bmp.Write("output.bmp");

    // Создание OpenGL-текстуры
    Texture texture(image);
    texture.createTexture();

    // === ОСНОВНОЙ ЦИКЛ РЕНДЕРИНГА ===
    bool running = true;
    SDL_Event event;
    while (running) {
      // Обработка событий SDL (закрытие окна, клавиатура и т.д.)
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;  // Выход при закрытии окна
        }
      }

      glRenderer.Render(shader, texture);
      // Обмен буферов: вывод кадра на экран
      SDL_GL_SwapWindow(window);
    }
    TextureManager::GetInstance().Shutdown();
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    TextureManager::GetInstance().Shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  // === ОЧИСТКА РЕСУРСОВ ===
  SDL_DestroyWindow(window);  // Удаление окна
  SDL_Quit();                 // Завершение SDL
  return 0;
}
