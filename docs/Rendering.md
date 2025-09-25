# Инструкция по запуску проекта Engine

Этот документ описывает, как настроить и запустить проект `Engine` на Windows. Проект использует SDL2, OpenGL 3.3, GLM и glad для рендеринга синего экрана (задачи Недели 1). Все необходимые файлы (код, шейдеры, glad) уже находятся в репозитории.

## Требования

- **Инструменты**:
  - CMake (версия 3.10 или выше): [https://cmake.org/download/](https://cmake.org/download/)
  - Git: [https://git-scm.com/download/win](https://git-scm.com/download/win)
  - vcpkg: [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)

## Установка зависимостей

### 1. Установите CMake

1. Скачайте CMake: [https://cmake.org/download/](https://cmake.org/download/).
2. Установите и добавьте CMake в PATH (выберите опцию "Add CMake to the system PATH" во время установки).
3. Проверьте установку:
   ```powershell
   cmake --version
   ```
   - Ожидаемый вывод: `cmake version 3.XX.X`.

### 2. Установите Git

1. Скачайте и установите Git: [https://git-scm.com/download/win](https://git-scm.com/download/win).
2. Проверьте установку:
   ```powershell
   git --version
   ```
   - Ожидаемый вывод: `git version 2.XX.X`.

### 3. Установите vcpkg

1. Клонируйте репозиторий vcpkg в любую папку на вашем компьютере (например, `C:\vcpkg`):
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git <your-vcpkg-path>
   cd <your-vcpkg-path>
   ```
   - Замените `<your-vcpkg-path>` на ваш путь (например, `C:\vcpkg`).
2. Запустите bootstrap:
   ```powershell
   .\bootstrap-vcpkg.bat
   ```
   - Если возникает ошибка "WinHttpSendRequest (0x00002F8F)", скачайте `vcpkg.exe` вручную с [https://github.com/microsoft/vcpkg-tool/releases](https://github.com/microsoft/vcpkg-tool/releases) и поместите в `<your-vcpkg-path>`.
3. Интегрируйте vcpkg:
   ```powershell
   .\vcpkg.exe integrate project
   ```
4. Установите библиотеки SDL2 и GLM:
   ```powershell
   .\vcpkg.exe install sdl2:x64-windows glm:x64-windows
   ```
5. Проверьте установку:
   ```powershell
   dir <your-vcpkg-path>\installed\x64-windows\lib\SDL2*
   dir <your-vcpkg-path>\installed\x64-windows\include\glm
   ```
   - Замените `<your-vcpkg-path>` на ваш путь к vcpkg.

## Клонирование репозитория

1. Клонируйте репозиторий в любую папку на вашем компьютере:
   ```powershell
   git clone https://github.com/Kopytkov/Engine.git <your-project-path>
   cd <your-project-path>
   ```
   - Замените `<your-project-path>` на путь, куда вы хотите клонировать проект (например, `C:\Users\<your-username>\Engine`).
2. Переключитесь на ветку `feature/render-cube`:
   ```powershell
   git checkout feature/render-cube
   ```

## Сборка проекта

### Вариант 1: Через PowerShell

1. Создайте и перейдите в папку `build`:
   ```powershell
   mkdir build
   cd build
   ```
2. Сконфигурируйте проект:
   ```powershell
   cmake .. -DCMAKE_TOOLCHAIN_FILE=<your-vcpkg-path>\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 17 2022" -A x64
   ```
   - Замените `<your-vcpkg-path>` на ваш путь к vcpkg (например, `C:\vcpkg`).
3. Соберите проект:
   ```powershell
   cmake --build . --config Release
   ```
4. Запустите программу:
   ```powershell
   .\Release\Engine.exe
   ```
   - **Ожидаемый результат**: Открывается окно 800x600 с синим фоном, которое закрывается по крестику.

### Вариант 2: Через VSCode

1. Откройте проект в VSCode:
   ```powershell
   cd <your-project-path>
   code .
   ```
   - Замените `<your-project-path>` на путь к проекту (например, `C:\Users\<your-username>\Engine`).
2. Установите расширения:
   - `ms-vscode.cmake-tools`
   - `ms-vscode.cpptools`
   ```powershell
   code --install-extension ms-vscode.cmake-tools
   code --install-extension ms-vscode.cpptools
   ```
3. Создайте файл `.vscode/settings.json`:
   ```powershell
   New-Item .vscode\settings.json -Force
   code .vscode\settings.json
   ```
   Вставьте:
   ```json
   {
     "cmake.configureArgs": [
       "-DCMAKE_TOOLCHAIN_FILE=<your-vcpkg-path>\\scripts\\buildsystems\\vcpkg.cmake"
     ],
     "cmake.generator": "Visual Studio 17 2022",
     "cmake.buildDirectory": "${workspaceFolder}/build",
     "cmake.configureOnOpen": true,
     "cmake.buildArgs": ["--config", "Release"]
   }
   ```
   - Замените `<your-vcpkg-path>` на ваш путь к vcpkg (например, `C:\\vcpkg`).
4. Сохраните файл и перезапустите VSCode.
5. Сконфигурируйте проект:
   - Нажмите `Ctrl+Shift+P` → выберите `CMake: Configure`.
6. Соберите и запустите:
   - Нажмите `Ctrl+Shift+P` → выберите `CMake: Build`.
   - Затем `Ctrl+Shift+P` → выберите `CMake: Run Without Debugging`.
   - **Ожидаемый результат**: Окно 800x600 с синим фоном.

### Дополнительно: Настройка горячей клавиши (необязательно)

1. Создайте файл `.vscode/tasks.json`:
   ```powershell
   New-Item .vscode\tasks.json -Force
   code .vscode\tasks.json
   ```
   Вставьте:
   ```json
   {
     "version": "2.0.0",
     "tasks": [
       {
         "label": "Build and Run",
         "type": "shell",
         "command": ".\\Release\\Engine.exe",
         "options": {
           "cwd": "${workspaceFolder}/build"
         },
         "group": {
           "kind": "build",
           "isDefault": true
         },
         "dependsOn": ["Build Project"],
         "problemMatcher": [],
         "detail": "Build and run the project"
       },
       {
         "label": "Build Project",
         "type": "shell",
         "command": "cmake",
         "args": ["--build", ".", "--config", "Release"],
         "options": {
           "cwd": "${workspaceFolder}/build"
         },
         "dependsOn": ["Configure CMake"],
         "problemMatcher": ["$msCompile"],
         "detail": "Build the project using CMake"
       },
       {
         "label": "Configure CMake",
         "type": "shell",
         "command": "cmake",
         "args": [
           "..",
           "-DCMAKE_TOOLCHAIN_FILE=<your-vcpkg-path>\\scripts\\buildsystems\\vcpkg.cmake",
           "-G",
           "Visual Studio 17 2022",
           "-A",
           "x64"
         ],
         "options": {
           "cwd": "${workspaceFolder}/build"
         },
         "problemMatcher": [],
         "detail": "Configure CMake with vcpkg"
       }
     ]
   }
   ```
   - Замените `<your-vcpkg-path>` на ваш путь к vcpkg (например, `C:\\vcpkg`).
2. Привяжите задачу к горячей клавише `F7`:
   - Нажмите `Ctrl+K, Ctrl+S` → найдите `Tasks: Run Build Task`.
   - Нажмите на карандаш → введите `F7` → подтвердите.
3. Нажмите `F7` для сборки и запуска.
   - **Ожидаемый результат**: Окно 800x600 с синим фоном.

## Возможные проблемы и решения

- **Ошибка: "Could not find SDL2Config.cmake"**:

  - Убедитесь, что `vcpkg` установлен и интегрирован:
    ```powershell
    <your-vcpkg-path>\vcpkg.exe integrate project
    ```
  - Проверьте наличие библиотек:
    ```powershell
    dir <your-vcpkg-path>\installed\x64-windows\lib\SDL2*
    dir <your-vcpkg-path>\installed\x64-windows\include\glm
    ```
  - Убедитесь, что путь к `vcpkg.cmake` правильный в `settings.json` или команде `cmake`.

- **Ошибка: "Failed to open file: assets/shaders/vertex.glsl"**:

  - Проверьте наличие файлов:
    ```powershell
    dir <your-project-path>\assets\shaders
    ```
  - Убедитесь, что `CMakeLists.txt` копирует шейдеры:
    ```cmake
    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/Release)
    ```
  - Или проверьте, что `main.cpp` устанавливает рабочую директорию с помощью `_chdir`.

- **Ошибка: "error C5145: must write to gl_Position"**:

  - Проверьте содержимое шейдеров:
    ```powershell
    type <your-project-path>\assets\shaders\vertex.glsl
    ```
    Должно быть:
    ```glsl
    #version 330 core
    layout(location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
    ```

- **Ошибка: "chdir: идентификатор не найден"**:
  - Убедитесь, что в `main.cpp` есть `#include <direct.h>` и используется `_chdir`.

## Результат

После успешной сборки и запуска (`.\Release\Engine.exe` или `F7` в VSCode) вы увидите окно 800x600 с синим фоном, которое закрывается по крестику.

## Дополнительные ресурсы

- [SDL2](https://www.libsdl.org/)
- [GLM](https://github.com/g-truc/glm)
- [LearnOpenGL](https://learnopengl.com/)
- [CMake Tools](https://github.com/microsoft/vscode-cmake-tools)

Если возникнут вопросы, обратитесь к команде через Pull Request или Issues в репозитории.
