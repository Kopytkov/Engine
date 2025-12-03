#include <SDL2/SDL.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "bmp/bmp.h"
#include "gl/gl_renderer.h"
#include "gl/shader.h"
#include "render/raw_image.h"
#include "render/renderer.h"
#include "render/scene_loader.h"
#include "render/texture.h"

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

  // Проверка и генерация текстур через манифест
  const std::string textures_dir = "assets/textures";
  const std::string sphere_json_path = "assets/scene/objects/sphere.json";
  const std::string manifest_path = "assets/textures/textures_manifest.json";
  const std::string python_script_cmd =
      "python assets/textures/script_for_textures.py";

  bool need_generate = false;

  try {
    // Получаем список всех ожидаемых имён объектов из sphere.json
    std::ifstream sphere_file(sphere_json_path);
    if (!sphere_file.is_open()) {
      throw std::runtime_error("Cannot open sphere.json");
    }
    json sphere_data = json::parse(sphere_file);

    std::vector<std::string> expected_names;
    for (const auto& item : sphere_data) {
      if (item.contains("name") && item["name"].is_string()) {
        expected_names.emplace_back(item["name"].get<std::string>());
      }
    }

    // Загружаем манифест (если есть)
    json manifest = json::object();  // пустой объект по умолчанию
    if (std::filesystem::exists(manifest_path)) {
      std::ifstream mf(manifest_path);
      if (mf) {
        mf >> manifest;  // nlohmann сам кидает исключение
      }
    } else {
      need_generate = true;  // манифеста нет — точно нужно генерировать
    }

    // Проверяем каждую текстуру
    if (!need_generate) {
      for (const auto& name : expected_names) {
        if (!manifest.contains(name)) {
          need_generate = true;
          break;
        }

        std::string rel_path = manifest[name];
        std::filesystem::path full_path =
            std::filesystem::path(textures_dir) / rel_path;

        if (!std::filesystem::exists(full_path)) {
          need_generate = true;
          break;
        }
      }
    }

    // --- Если чего-то не хватает — запускаем Python-скрипт ---
    if (need_generate) {
      int result = std::system(python_script_cmd.c_str());
      if (result != 0) {
        std::cerr << "[Textures] Python script failed (code " << result
                  << "). Continuing anyway.\n";
        return -1;
      }
    }

  } catch (const std::exception& e) {
    std::cerr << "[Textures] Error during texture check: " << e.what() << "\n";
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
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  // === ОЧИСТКА РЕСУРСОВ ===
  SDL_DestroyWindow(window);  // Удаление окна
  SDL_Quit();                 // Завершение SDL
  return 0;
}
