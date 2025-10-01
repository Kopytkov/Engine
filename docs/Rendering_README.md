# Описание функционала файлов проекта Engine

Этот документ описывает назначение и функционал файлов в проекте `Engine`, созданного для задач Недели 1 (рендеринг синего экрана с использованием SDL2, OpenGL 3.3, GLM и glad). Проект находится в ветке `feature/render-cube`.

## Структура проекта (будет измениться со временем)

```
Engine/
├── assets/
│   └── shaders/
│       ├── vertex.glsl
│       └── fragment.glsl
├── docs/
│   ├── Rendering.md
│   ├── Rendering_README.md
│   └── setup.md
├── include/
│   └── shader.h
├── lib/
│   └── glad/
│       ├── include/
│       │   ├── glad/
│       │   │   └── glad.h
│       │   └── KHR/
│       │       └── khrplatform.h
│       └── src/
│           └── glad.c
├── src/
│   ├── main.cpp
│   └── shader.cpp
├── .gitignore
├── .vscode/
│   ├── settings.json
│   └── tasks.json
└── CMakeLists.txt
```

## Описание файлов

### 1. `CMakeLists.txt`

- **Назначение**: Конфигурационный файл для CMake, определяющий сборку проекта.
- **Функционал**:
  - Находит библиотеки SDL2 и GLM через `find_package` (vcpkg подключается через аргумент `-DCMAKE_TOOLCHAIN_FILE`).
  - Создаёт статическую библиотеку `glad` из `lib/glad/src/glad.c`.
  - Создаёт исполняемый файл `Engine` из `src/main.cpp` и `src/shader.cpp`.
  - Линкует библиотеки: `SDL2`, `GLM`, `glad`.
- **Ключевые команды**:
  ```cmake
  find_package(SDL2 CONFIG REQUIRED)
  find_package(glm CONFIG REQUIRED)
  add_library(glad STATIC lib/glad/src/glad.c)
  add_executable(Engine src/main.cpp src/shader.cpp)
  ```

### 2. `src/main.cpp`

- **Назначение**: Точка входа программы, инициализирует SDL2, OpenGL и рендеринг.
- **Функционал**:
  - Инициализирует SDL2 (`SDL_Init(SDL_INIT_VIDEO)`).
  - Настраивает OpenGL 3.3 Core Profile (`SDL_GL_SetAttribute`).
  - Создаёт окно 800x600 (`SDL_CreateWindow`).
  - Инициализирует OpenGL-контекст (`SDL_GL_CreateContext`) и glad (`gladLoadGLLoader`).
  - Устанавливает рабочую директорию в корень проекта с помощью `_chdir` для корректной загрузки шейдеров.
  - Загружает шейдеры через класс `Shader` (`assets/shaders/vertex.glsl`, `assets/shaders/fragment.glsl`).
  - Запускает основной цикл рендеринга:
    - Обрабатывает события (закрытие окна через `SDL_QUIT`).
    - Очищает экран синим цветом (`glClearColor(0.0f, 0.0f, 1.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT)`).
    - Активирует шейдер (`shader.use()`).
    - Обновляет окно (`SDL_GL_SwapWindow`).
  - Очищает ресурсы при выходе (`SDL_GL_DeleteContext`, `SDL_DestroyWindow`, `SDL_Quit`).
- **Ключевые функции**:
  - `SDL_Init`, `SDL_CreateWindow`, `SDL_GL_CreateContext`: Инициализация SDL и OpenGL.
  - `gladLoadGLLoader`: Загрузка OpenGL-функций через glad.
  - `_chdir`: Установка рабочей директории для доступа к `assets/shaders`.
  - `glClearColor`, `glClear`: Установка синего фона.

### 3. `include/shader.h`

- **Назначение**: Заголовочный файл с определением класса `Shader`.
- **Функционал**:
  - Объявляет класс `Shader` для загрузки, компиляции и использования шейдеров OpenGL.
  - **Конструктор**: Принимает пути к вершинному и фрагментному шейдерам.
  - **Метод `use`**: Активирует шейдерную программу (`glUseProgram`).
  - **Деструктор**: Очищает шейдерную программу (`glDeleteProgram`).
  - **Приватные методы**:
    - `readFile`: Читает код шейдера из файла.
    - `compileShader`: Компилирует шейдер (вершинный или фрагментный).
  - **Переменная**: `programID` хранит ID шейдерной программы.
- **Ключевые элементы**:
  ```cpp
  class Shader {
  public:
      Shader(const std::string& vertexPath, const std::string& fragmentPath);
      ~Shader();
      void use() const;
  private:
      GLuint programID;
      std::string readFile(const std::string& path);
      GLuint compileShader(const std::string& source, GLenum type);
  };
  ```

### 4. `src/shader.cpp`

- **Назначение**: Реализация класса `Shader` для работы с шейдерами.
- **Функционал**:
  - **Конструктор**:
    - Читает код шейдеров из файлов (`readFile`).
    - Компилирует вершинный и фрагментный шейдеры (`compileShader`).
    - Создаёт и линкует шейдерную программу (`glCreateProgram`, `glLinkProgram`).
    - Проверяет ошибки компиляции и линковки, выводит их в `std::cerr`.
  - **Метод `use`**: Активирует программу, если она валидна (`glUseProgram`).
  - **Деструктор**: Удаляет программу (`glDeleteProgram`).
  - **Метод `readFile`**: Читает файл шейдера в строку с помощью `std::ifstream`.
  - **Метод `compileShader`**: Компилирует шейдер (`glCreateShader`, `glShaderSource`, `glCompileShader`) и проверяет ошибки.
- **Ключевые функции**:
  - `glCreateShader`, `glShaderSource`, `glCompileShader`: Компиляция шейдеров.
  - `glCreateProgram`, `glAttachShader`, `glLinkProgram`: Создание программы.
  - `glGetShaderInfoLog`, `glGetProgramInfoLog`: Диагностика ошибок.

### 5. `assets/shaders/vertex.glsl`

- **Назначение**: Вершинный шейдер для обработки вершин.
- **Функционал**:
  - Определяет версию OpenGL 3.3 (`#version 330 core`).
  - Принимает входной атрибут `aPos` (координаты вершин).
  - Устанавливает `gl_Position` для передачи координат в OpenGL.
- **Код**:
  ```glsl
  #version 330 core
  layout(location = 0) in vec3 aPos;
  void main() {
      gl_Position = vec4(aPos, 1.0);
  }
  ```

### 6. `assets/shaders/fragment.glsl`

- **Назначение**: Фрагментный шейдер для установки цвета пикселей.
- **Функционал**:
  - Определяет версию OpenGL 3.3 (`#version 330 core`).
  - Задаёт выходной цвет `fragColor` (синий: RGB = 0.0, 0.0, 1.0, альфа = 1.0).
- **Код**:
  ```glsl
  #version 330 core
  out vec4 fragColor;
  void main() {
      fragColor = vec4(0.0, 0.0, 1.0, 1.0);
  }
  ```

### 7. `lib/glad/`

- **Назначение**: Библиотека glad для загрузки функций OpenGL.
- **Файлы**:
  - `lib/glad/include/glad/glad.h`: Заголовок с определениями OpenGL-функций.
  - `lib/glad/include/KHR/khrplatform.h`: Платформенные определения.
  - `lib/glad/src/glad.c`: Реализация загрузчика OpenGL.
- **Функционал**:
  - Инициализируется в `main.cpp` через `gladLoadGLLoader`.
  - Предоставляет доступ к функциям OpenGL 3.3 Core Profile.

### 8. `docs/Rendering.md`

- **Назначение**: Инструкция по установке, сборке и запуску проекта.
- **Функционал**: Описывает шаги для настройки окружения (Visual Studio, CMake, vcpkg), клонирования репозитория, сборки через PowerShell или VSCode, и устранения типичных ошибок.

### 9. `.vscode/settings.json`

- **Назначение**: Настройки VSCode для интеграции с CMake Tools.
- **Функционал**:
  - Указывает путь к `vcpkg.cmake` через `cmake.configureArgs`.
  - Задаёт генератор (`Visual Studio 17 2022`) и папку сборки (`build`).
  - Автоматизирует конфигурацию (`cmake.configureOnOpen`).
  - Устанавливает режим сборки `Release`.

### 10. `.vscode/tasks.json` (по желанию)

- **Назначение**: Задачи для сборки и запуска проекта в VSCode.
- **Функционал**:
  - Задача `Configure CMake`: Выполняет `cmake ..` с `vcpkg`.
  - Задача `Build Project`: Выполняет `cmake --build . --config Release`.
  - Задача `Build and Run`: Запускает `.\Release\Engine.exe`.
  - Позволяет собирать и запускать проект одной командой (`F7`, если настроена).

### 11. `.gitignore`

- **Назначение**: Игнорирует временные файлы и папку `build`.
- **Функционал**:
  - Исключает из Git: `build/`, `*.o`, `*.exe`, `*.log`.

## Результат

Проект инициализирует окно SDL2 800x600, устанавливает OpenGL 3.3 Core Profile, загружает шейдеры и рендерит синий экран. Шейдеры подготавливают основу для рендеринга геометрии (например, куба в Неделе 2).

## Дополнительные ресурсы

- [SDL2](https://www.libsdl.org/)
- [GLM](https://github.com/g-truc/glm)
- [LearnOpenGL](https://learnopengl.com/)
