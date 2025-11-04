cmake_minimum_required(VERSION 3.10)
project(Engine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

add_executable(Engine
    src/main.cpp
    src/bmp/bmp.cpp src/bmp/bmp.h
    src/gl/shader.cpp src/gl/shader.h
    src/gl/gl_renderer.cpp src/gl/gl_renderer.h
    src/math/vec.h src/math/vec_functions.h
    src/render/camera.cpp src/render/camera.h
    src/render/material.h
    src/render/ray.cpp src/render/ray.h
    src/render/raw_image.cpp src/render/raw_image.h
    src/render/render.h
    src/render/renderer.cpp src/render/renderer.h
    src/render/scene_loader.cpp src/render/scene_loader.h
    src/render/scene_object.cpp src/render/scene_object.h
    src/render/scene_object_sphere.cpp src/render/scene_object_sphere.h
    src/render/scene.cpp src/render/scene.h
    src/render/light_source.cpp src/render/light_source.h
    src/render/light_source_point.cpp src/render/light_source_point.h
    src/render/light_source_global.cpp src/render/light_source_global.h
    src/render/texture.cpp src/render/texture.h
    src/render/RGB.cpp src/render/RGB.h
)

target_link_libraries(Engine 
    PRIVATE
        mingw32
        SDL2::SDL2main
        SDL2::SDL2
        glm::glm
        glad
        nlohmann_json::nlohmann_json
)

target_include_directories(Engine PRIVATE src)
