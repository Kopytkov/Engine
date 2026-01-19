#pragma once
#include <glad/glad.h>
#include "raw_image.h"

class Texture {
 public:
  explicit Texture(const RawImage& image);
  ~Texture();

  // Запрещаем копирование
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  // Разрешаем перемещение
  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  // Создаёт OpenGL-текстуру из данных изображения
  void createTexture();

  // Обновляет данные текстуры из переданного RawImage
  void updateTexture(const RawImage& image);

  // Привязывает созданную текстуру к текущему контексту OpenGL
  void bindTexture() const;

  // Возвращает ID текстуры для использования в рендеринге
  GLuint getTextureID() const { return textureID_; }

  RawImage& getImage() { return image_; }              // для модификации
  const RawImage& getImage() const { return image_; }  // для чтения

 private:
  RawImage image_;    // Копия RawImage
  GLuint textureID_;  // ID OpenGL-текстуры
};
