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

# Добавление nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

# Основной исполняемый файл
add_executable(Engine
 src/main.cpp
 src/bmp/bmp.h
 src/bmp/bmp.cpp
 src/gl/shader.h
 src/gl/shader.cpp
 src/gl/gl_renderer.h
 src/gl/gl_renderer.cpp
 src/math/vec.h
 src/math/vec_functions.h
 src/physics/physics_body.h
 src/physics/physics_body.cpp
 src/physics/physics_engine.h
 src/physics/physics_engine.cpp
 src/physics/physics_material.h
 src/render/app_utils.h
 src/render/app_utils.cpp
 src/render/camera.h
 src/render/camera.cpp
 src/render/camera_parser.h
 src/render/camera_parser.cpp
 src/render/json_utils.h
 src/render/material_pbr.h
 src/render/material_pbr.cpp
 src/render/material_phong.h
 src/render/material_phong.cpp
 src/render/material.h
 src/render/material.cpp
 src/render/object_parser.h
 src/render/object_parser.cpp
 src/render/ray.h
 src/render/ray.cpp
 src/render/raw_image.h
 src/render/raw_image.cpp
 src/render/renderer.h
 src/render/renderer.cpp
 src/render/scene_entity.h
 src/render/scene_loader.h
 src/render/scene_loader.cpp
 src/render/scene_object.h
 src/render/scene_object.cpp
 src/render/scene_object_sphere.h
 src/render/scene_object_sphere.cpp
 src/render/scene.h
 src/render/scene.cpp
 src/render/light_parser.h
 src/render/light_parser.cpp
 src/render/light_source.h
 src/render/light_source.cpp
 src/render/light_source_point.h
 src/render/light_source_point.cpp
 src/render/light_source_global.h
 src/render/light_source_global.cpp
 src/render/texture.h
 src/render/texture.cpp
 src/render/texture_manager.h
 src/render/texture_manager.cpp
 src/render/RGB.h
 src/render/RGB.cpp
 src/render/scene_object_box.h
 src/render/scene_object_box.cpp
 src/control/property.h
 src/control/property.cpp
 src/control/controller.h
 src/control/controller.cpp
 )
target_link_libraries(Engine 
    PRIVATE 
        SDL2::SDL2 
        SDL2::SDL2main 
        glm::glm 
        glad 
        nlohmann_json::nlohmann_json
    )
target_include_directories(Engine PRIVATE src)
