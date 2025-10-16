#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include "bmp/bmp.h"
#include "render/render.h"
#include "gl/shader.h"
#include "render/raw_image.h"

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

    // Создание контекста OpenGL
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Инициализация glad для загрузки OpenGL-функций
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        SDL_GL_DeleteContext(glContext);
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

    // Создание OpenGL-текстуры из RawImage
    image.createTexture();

    // Настройка геометрии квада (два треугольника) для отображения текстуры
    float vertices[] = {
        // Позиции вершин        // Текстурные координаты
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // Левый нижний угол
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // Правый нижний угол
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // Правый верхний угол
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f // Левый верхний угол
    };
    unsigned int indices[] = {
        0, 1, 2, // Первый треугольник
        2, 3, 0  // Второй треугольник
    };

    // === НАСТРОЙКА ГЕОМЕТРИИ ПОЛНОЭКРАННОГО КВАДА (два треугольника) ===
    // Квад состоит из двух треугольников, покрывающих весь экран в нормализованных координатах (-1..1).

    GLuint VAO, VBO, EBO;  // Идентификаторы: VAO — Vertex Array Object, VBO — Vertex Buffer Object, EBO — Element Buffer Object

    // Генерация идентификаторов для VAO, VBO и EBO
    glGenVertexArrays(1, &VAO);  // Создаёт объект, который хранит состояние вершинных атрибутов
    glGenBuffers(1, &VBO);       // Создаёт буфер для хранения данных вершин (позиции + текстурные координаты)
    glGenBuffers(1, &EBO);       // Создаёт буфер для хранения индексов (порядок отрисовки вершин)

    // Привязка VAO — все последующие операции с буферами будут сохранены в этом VAO
    glBindVertexArray(VAO);

    // Привязка VBO и загрузка данных вершин (позиции + текстурные координаты)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // GL_STATIC_DRAW: данные загружаются один раз и используются многократно (не меняются)

    // Привязка EBO и загрузка индексов (определяет, как соединять вершины в треугольники)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // === НАСТРОЙКА АТРИБУТОВ ВЕРШИН ===
    // Атрибут 0: позиция вершины (3 float: x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Параметры: index=0, size=3, type=GL_FLOAT, normalized=GL_FALSE, stride=5*sizeof(float), pointer=0
    // Stride: расстояние между вершинами в массиве (3 float позиция + 2 float UV = 5 float)

    // Атрибут 1: текстурные координаты (2 float: u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // Смещение: 3 * sizeof(float) — пропускаем позицию, начинаем с UV

    // Отвязка VAO — состояние сохранено, можно использовать позже
    glBindVertexArray(0);

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

        // Очистка цветового буфера (экран становится чёрным)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Установка цвета очистки: чёрный
        glClear(GL_COLOR_BUFFER_BIT);          // Выполнение очистки

        // === РЕНДЕРИНГ ТЕКСТУРЫ НА КВАД ===
        shader.use();  // Активация шейдерной программы

        // Активация текстурного юнита 0 (GL_TEXTURE0) — OpenGL поддерживает несколько юнитов
        glActiveTexture(GL_TEXTURE0);
        image.bindTexture();  // Привязка текстуры из RawImage к активному юниту

        // Установка uniform-переменной в шейдере: texture1 использует юнит 0
        shader.setInt("texture1", 0);

        // Привязка VAO — восстанавливаем состояние вершинных данных
        glBindVertexArray(VAO);
        // Отрисовка: 6 индексов = 2 треугольника
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);  // Отвязка VAO

        // Обмен буферов: вывод кадра на экран
        SDL_GL_SwapWindow(window);
    }

    // === ОЧИСТКА РЕСУРСОВ ===
    glDeleteVertexArrays(1, &VAO);  // Удаление VAO
    glDeleteBuffers(1, &VBO);       // Удаление VBO
    glDeleteBuffers(1, &EBO);       // Удаление EBO
    SDL_GL_DeleteContext(glContext); // Удаление OpenGL-контекста
    SDL_DestroyWindow(window);       // Удаление окна
    SDL_Quit();                      // Завершение SDL
    return 0;
}