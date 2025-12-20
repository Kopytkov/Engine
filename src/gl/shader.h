#pragma once
#include <glad/glad.h>
#include <string>
#include "math/vec.h"

class Shader {
 public:
  Shader(const std::string& vertexPath, const std::string& fragmentPath);
  ~Shader();

  void use() const;

  void setInt(const std::string& name, int value) const;
  void setFloat(const std::string& name, float value) const;
  void setVec2(const std::string& name, float x, float y) const;
  void setVec3(const std::string& name, float x, float y, float z) const;
  void setVec3(const std::string& name, const vec3& value) const;

  GLuint getID() const { return programID; }

 private:
  GLuint programID;

  std::string readFile(const std::string& path);
  GLuint compileShader(const std::string& source, GLenum type);
};
