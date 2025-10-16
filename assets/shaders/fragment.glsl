#version 330 core
// Входные атрибуты: текстурные координаты от вершинного шейдера
// Выходные данные: цвет фрагмента
   out vec4 fragColor;
   in vec2 TexCoord;
   uniform sampler2D texture1;
   void main() {
       fragColor = texture(texture1, TexCoord);
   }