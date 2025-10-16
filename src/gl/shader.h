#pragma once
#include <glad/glad.h>
#include <string>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    void use() const;
    GLuint getID() const { return programID; }

private:
    GLuint programID;
    std::string readFile(const std::string& path);
    GLuint compileShader(const std::string& source, GLenum type);
};