#include <SDL2/SDL.h>
#include <iostream>
#include "bmp/bmp.h"
#include "render/render.h"
#include "gl/shader.h"
#include "gl/gl_renderer.h"
#include "render/raw_image.h"
#include "render/texture.h"

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

    // Инициализация GLRenderer
    GLRenderer glRenderer(window);
    if (!glRenderer.Initialize()) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Загрузка шейдеров
    Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    // Рендеринг сцены в RawImage
    RawImage image(800, 600);
    Camera camera(vec3(0, -20, 0), vec3(0, 1, 0), vec3(0, 0, 1), image.GetWidth(), image.GetHeight(), 40, 75);
    Scene scene;
    scene.AddObject(std::make_unique<Sphere>(vec3(0, 0, 0), 5.0f));
    scene.AddLight(std::make_unique<PointLightSource>(vec3(-10, -10, 10), 1.0f, RGB{255, 255, 255}));
    Renderer renderer;
    renderer.Render(camera, scene, image);

    // Сохранение изображения в BMP файл
    BMP bmp(image);
    std::ofstream out("temp1.bmp");
    bmp.Write(out);
    out.close();

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

    // === ОЧИСТКА РЕСУРСОВ ===
    SDL_DestroyWindow(window);       // Удаление окна
    SDL_Quit();                      // Завершение SDL
    return 0;
}