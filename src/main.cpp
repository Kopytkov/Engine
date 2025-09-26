#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <direct.h> // Для _chdir на Windows
#include "shader.h"
#include "json_parser.h"

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
    SDL_Window* window = SDL_CreateWindow(
        "Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Создание контекста
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Инициализация glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize glad" << std::endl;
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Установить рабочую директорию в корень проекта
    char* base_path = SDL_GetBasePath();
    if (base_path) {
        std::string project_root = base_path;
        project_root = project_root.substr(0, project_root.find_last_of("\\/"));
        project_root = project_root.substr(0, project_root.find_last_of("\\/"));
        project_root = project_root.substr(0, project_root.find_last_of("\\/"));
        SDL_free(base_path);
        if (_chdir(project_root.c_str()) != 0) {
            std::cerr << "Failed to set working directory to: " << project_root << std::endl;
        }
    }

    // Тест JSON-парсера
    try {
        Object obj = parseObject("assets/object.json");
        std::cout << "Parsed object: " << obj.name << "\n";
        std::cout << "Position: (" << obj.pos.x << ", " << obj.pos.y << ", " << obj.pos.z << ")\n";
        std::cout << "Scale: (" << obj.scale.x << ", " << obj.scale.y << ", " << obj.scale.z << ")\n";
        std::cout << "Texture: " << obj.texture << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    // Загрузка шейдеров
    Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    // Основной цикл
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Рендеринг синего экрана
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        SDL_GL_SwapWindow(window);
    }

    // Очистка
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}