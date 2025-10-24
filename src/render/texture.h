#pragma once
#include <glad/glad.h>
#include "raw_image.h"

class Texture {
 public:
  explicit Texture(const RawImage& image);
  ~Texture();

  // Создаёт OpenGL-текстуру из данных изображения
  void createTexture();

  // Обновляет данные текстуры из переданного RawImage
  void updateTexture(const RawImage& image);

  // Привязывает созданную текстуру к текущему контексту OpenGL
  void bindTexture() const;

  // Возвращает ID текстуры для использования в рендеринге
  GLuint getTextureID() const { return textureID_; }

 private:
  const RawImage& image_;  // Ссылка на RawImage для доступа к данным
  GLuint textureID_;       // ID OpenGL-текстуры
};
