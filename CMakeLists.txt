cmake_minimum_required(VERSION 3.10)
project(Engine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключение библиотек (позже добавим SDL2, GLM, glad)
add_executable(Engine src/main.cpp)