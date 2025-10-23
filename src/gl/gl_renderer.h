#pragma once
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include "../render/raw_image.h"
#include "shader.h"

class GLRenderer {
public:
    GLRenderer(SDL_Window* window);
    ~GLRenderer();

    bool Initialize();
    void Render(const Shader& shader, const RawImage& image);
    void BindTexture(const RawImage& image);

private:
    SDL_GLContext glContext_;
    GLuint VAO_, VBO_, EBO_;

    void SetupQuad();
};