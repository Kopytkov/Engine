#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    // Чтение шейдеров
    std::string vertexCode = readFile(vertexPath);
    if (vertexCode.empty()) {
        std::cerr << "Error: Vertex shader file is empty or not found: " << vertexPath << std::endl;
        programID = 0;
        return;
    }
    std::string fragmentCode = readFile(fragmentPath);
    if (fragmentCode.empty()) {
        std::cerr << "Error: Fragment shader file is empty or not found: " << fragmentPath << std::endl;
        programID = 0;
        return;
    }

    // Компиляция шейдеров
    GLuint vertexShader = compileShader(vertexCode, GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        programID = 0;
        return;
    }
    GLuint fragmentShader = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        programID = 0;
        return;
    }

    // Создание программы
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // Проверка ошибок линковки
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(programID);
        programID = 0;
    }

    // Удаление шейдеров
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void Shader::use() const {
    if (programID != 0) {
        glUseProgram(programID);
    }
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(const std::string& source, GLenum type) {
    if (source.empty()) {
        std::cerr << "Error: Shader source is empty for " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        return 0;
    }

    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Проверка ошибок компиляции
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "):\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}