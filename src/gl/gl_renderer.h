#pragma once
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "../render/texture.h"
#include "shader.h"

class GLRenderer {
 public:
  GLRenderer(SDL_Window* window);
  ~GLRenderer();

  bool Initialize();
  void Render(const Shader& shader, const Texture& texture);
  void BindTexture(const Texture& texture);

 private:
  SDL_GLContext glContext_;
  GLuint VAO_, VBO_, EBO_;

  void SetupQuad();
};
