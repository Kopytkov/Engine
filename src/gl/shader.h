#pragma once
#include <glad/glad.h>
#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Активирует шейдерную программу для рендеринга
    void use() const;

    // Устанавливает целочисленное значение для uniform-переменной в шейдере
    void setInt(const std::string& name, int value) const;
    GLuint getID() const { return programID; }

private:
    GLuint programID;

    // Вспомогательные функции для чтения файлов и компиляции шейдеров
    std::string readFile(const std::string& path);

    // Компилирует шейдер указанного типа (вершинный или фрагментный)
    GLuint compileShader(const std::string& source, GLenum type);
};