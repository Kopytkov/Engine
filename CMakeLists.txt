cmake_minimum_required(VERSION 3.10)
project(Engine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотек
find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Добавление glad
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Основной исполняемый файл
add_executable(Engine
 src/main.cpp
 src/bmp/bmp.h
 src/bmp/bmp.cpp
 src/gl/shader.h
 src/gl/shader.cpp
 src/math/vec.h
 src/math/vec_functions.h
 src/render/camera.h
 src/render/camera.cpp
 src/render/ray.h
 src/render/ray.cpp
 src/render/raw_image.h
 src/render/raw_image.cpp
 src/render/render.h
 src/render/renderer.h
 src/render/renderer.cpp
 src/render/scene_object.h
 src/render/scene_object.cpp
 src/render/scene_object_sphere.h
 src/render/scene_object_sphere.cpp
 src/render/scene.h
 src/render/scene.cpp
 src/render/light_source.h
 src/render/light_source.cpp
 src/render/light_source_point.h
 src/render/light_source_point.cpp
 src/render/light_source_global.h
 src/render/light_source_global.cpp
)
target_link_libraries(Engine PRIVATE SDL2::SDL2 SDL2::SDL2main glm::glm glad)
target_include_directories(Engine PRIVATE src)