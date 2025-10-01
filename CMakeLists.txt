cmake_minimum_required(VERSION 3.10)
project(Engine LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск библиотек
find_package(SDL2 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Добавление glad
add_library(glad STATIC lib/glad/src/glad.c)
target_include_directories(glad PUBLIC lib/glad/include)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Основной исполняемый файл
add_executable(Engine src/main.cpp src/shader.cpp)
target_link_libraries(Engine PRIVATE SDL2::SDL2 SDL2::SDL2main glm::glm glad)
target_include_directories(Engine PRIVATE include)