#include "gl_renderer.h"
#include <iostream>
#include "render/app_utils.h"
#include "render/scene_entity.h"
#include "render/scene_object_sphere.h"

GLRenderer::GLRenderer(SDL_Window* window)
    : glContext_(nullptr), VAO_(0), VBO_(0), EBO_(0) {
  // Создание контекста OpenGL
  glContext_ = SDL_GL_CreateContext(window);
  if (!glContext_) {
    std::cerr << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
  }
}

GLRenderer::~GLRenderer() {
  // === Очистка ресурсов ===
  glDeleteVertexArrays(1, &VAO_);  // Удаление VAO
  glDeleteBuffers(1, &VBO_);       // Удаление VBO
  glDeleteBuffers(1, &EBO_);       // Удаление EBO
  if (glContext_) {
    SDL_GL_DeleteContext(glContext_);  // Удаление OpenGL-контекста
  }
}

bool GLRenderer::Initialize() {
  if (!glContext_) {
    return false;
  }

  // Инициализация glad для загрузки OpenGL-функций
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return false;
  }

  SetupQuad();
  return true;
}

void GLRenderer::Render(const Shader& shader, const Texture& texture) {
  // Очистка цветового буфера
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Установка цвета очистки: чёрный
  glClear(GL_COLOR_BUFFER_BIT);          // Выполнение очистки

  // === Рендеринг текстуры на квад ===
  shader.use();  // Активация шейдерной программы
  BindTexture(texture);
  shader.setInt(
      "texture1",
      0);  // Установка uniform-переменной в шейдере: texture1 использует юнит 0

  // Привязка VAO — восстанавливаем состояние вершинных данных
  glBindVertexArray(VAO_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                 0);     // Отрисовка: 6 индексов = 2 треугольника
  glBindVertexArray(0);  // Отвязка VAO
}

void GLRenderer::BindTexture(const Texture& texture) {
  glActiveTexture(GL_TEXTURE0);  // Активация текстурного юнита 0 (GL_TEXTURE0)
  texture.bindTexture();         // Привязка текстуры к активному юниту
}

void GLRenderer::UpdateUniforms(const Scene& scene, Shader& shader) {
  shader.use();

  // Итерация по всем сущностям
  const auto& entities = scene.GetEntities();
  for (const auto& entity : entities) {
    if (entity->object) {
      entity->object->UpdateUniforms(shader);
    }
  }

  // Собираем массив позиций для шаров
  std::vector<vec3> ballPositions;
  std::vector<mat3<float>> ballRotations;
  for (const auto& entity : entities) {
    if (entity->object) {
      if (auto* sphere = dynamic_cast<Sphere*>(entity->object.get())) {
        ballPositions.push_back(sphere->GetRenderPosition());
        quat q;
        if (entity->body) {
          q = entity->body->orientation;
        }
        ballRotations.push_back(quatToMat3(q));
      }
    }
  }

  // Передаём массив в шейдер
  int count = static_cast<int>(ballPositions.size());
  if (count > 16) {
    std::cerr << "Too many spheres: " << count << " (max 16)\n";
    count = 16;
  }

  for (int i = 0; i < count; ++i) {
    std::string idx = "[" + std::to_string(i) + "]";
    shader.setVec3("ballPositions" + idx, ballPositions[i]);
    mat3<float> R = ballRotations[i];
    float flatMat[9] = {
        R[0][0], R[0][1], R[0][2],  // 1-я строка
        R[1][0], R[1][1], R[1][2],  // 2-я строка
        R[2][0], R[2][1], R[2][2]   // 3-я строка
    };

    // Передаем как матрицу
    shader.setMat3("ballRotations" + idx, flatMat);
  }

  // Обновляем количество
  shader.setInt("ballCount", count);
}

void GLRenderer::SetupQuad() {
  // === Настройка геометрии полноэкранного квада (два треугольника) ===
  // Квад состоит из двух треугольников, покрывающих весь экран в
  // нормализованных координатах (-1..1).
  float vertices[] = {
      // Позиции вершин        // Текстурные координаты
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Левый нижний угол
      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,  // Правый нижний угол
      1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // Правый верхний угол
      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f   // Левый верхний угол
  };
  unsigned int indices[] = {
      0, 1, 2,  // Первый треугольник
      2, 3, 0   // Второй треугольник
  };

  // Генерация идентификаторов для VAO, VBO и EBO
  glGenVertexArrays(
      1,
      &VAO_);  // Создаёт объект, который хранит состояние вершинных атрибутов
  glGenBuffers(1, &VBO_);  // Создаёт буфер для хранения данных вершин (позиции
                           // + текстурные координаты)
  glGenBuffers(
      1,
      &EBO_);  // Создаёт буфер для хранения индексов (порядок отрисовки вершин)

  // Привязка VAO — все последующие операции с буферами будут сохранены в этом
  // VAO
  glBindVertexArray(VAO_);

  // Привязка VBO и загрузка данных вершин (позиции + текстурные координаты)
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
               GL_STATIC_DRAW);  // GL_STATIC_DRAW: данные загружаются один раз
                                 // и используются многократно

  // Привязка EBO и загрузка индексов (определяет, как соединять вершины в
  // треугольники)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // === Настройка атрибутов вершин ===
  // Атрибут 0: позиция вершины (3 float: x, y, z)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // Параметры: index=0, size=3, type=GL_FLOAT, normalized=GL_FALSE,
  // stride=5*sizeof(float), pointer=0 Stride: расстояние между вершинами в
  // массиве (3 float позиция + 2 float UV = 5 float)

  // Атрибут 1: текстурные координаты (2 float: u, v)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Смещение: 3 * sizeof(float) — пропускаем позицию, начинаем с UV

  // Отвязка VAO — состояние сохранено, можно использовать позже
  glBindVertexArray(0);
}
