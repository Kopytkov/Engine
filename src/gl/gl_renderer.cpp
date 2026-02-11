#include "gl_renderer.h"
#include <iostream>
#include "render/app_utils.h"
#include "render/material_converter.h"
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

  // === Создание UBO ===
  glGenBuffers(1, &uboBalls_);
  glBindBuffer(GL_UNIFORM_BUFFER, uboBalls_);
  // Выделяем память
  glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOData), nullptr, GL_DYNAMIC_DRAW);
  // Привязываем буфер к точке привязки (Binding Point) 0
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboBalls_);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

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

  // Связываем индекс блока в шейдере с точкой привязки 0
  unsigned int blockIndex = glGetUniformBlockIndex(shader.getID(), "BallBlock");
  glUniformBlockBinding(shader.getID(), blockIndex, 0);

  // Подготовка данных на CPU
  UBOData uboData;
  // Обнуляем память, чтобы в паддингах не было мусора
  std::memset(&uboData, 0, sizeof(UBOData));

  uboData.ballCount = 0;
  uboData.ballRadius = 0.0f;

  const auto& entities = scene.GetEntities();
  for (const auto& entity : entities) {
    if (!entity->object) {
      continue;
    }

    if (auto* sphere = dynamic_cast<Sphere*>(entity->object.get())) {
      if (uboData.ballCount >= 16) {  // Лимит массива в шейдере
        break;
      }

      int i = uboData.ballCount;

      if (uboData.ballRadius == 0.0f) {
        uboData.ballRadius = sphere->GetRadius();
      }

      // Копирование позиции
      vec3 pos = sphere->GetRenderPosition();
      uboData.balls[i].px = pos[0];
      uboData.balls[i].py = pos[1];
      uboData.balls[i].pz = pos[2];

      // Копирование матрицы вращения (Конвертация quat -> mat3 -> 3x vec4)
      quat q = (entity->body) ? entity->body->orientation : quat(1, 0, 0, 0);
      mat3<float> R = quatToMat3(q);
      // Столбец 0
      uboData.balls[i].rotCol0[0] = R[0][0];
      uboData.balls[i].rotCol0[1] = R[1][0];
      uboData.balls[i].rotCol0[2] = R[2][0];
      // Столбец 1
      uboData.balls[i].rotCol1[0] = R[0][1];
      uboData.balls[i].rotCol1[1] = R[1][1];
      uboData.balls[i].rotCol1[2] = R[2][1];
      // Столбец 2
      uboData.balls[i].rotCol2[0] = R[0][2];
      uboData.balls[i].rotCol2[1] = R[1][2];
      uboData.balls[i].rotCol2[2] = R[2][2];

      // Конвертация материала
      const Material& mat = sphere->GetMaterial();
      const MaterialPBR* pbrMat = dynamic_cast<const MaterialPBR*>(&mat);
      uboData.balls[i].material = MaterialConverter::Convert(pbrMat, i);

      uboData.ballCount++;
    }
  }

  if (uboData.ballRadius == 0.0f) {
    uboData.ballRadius = 1.35f;
  }

  // === Отправка данных в GPU ===
  glBindBuffer(GL_UNIFORM_BUFFER, uboBalls_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOData), &uboData);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
