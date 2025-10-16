#version 330 core
// Входные атрибуты: позиция вершины и текстурные координаты
// Выходные данные: текстурные координаты для фрагментного шейдера
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }