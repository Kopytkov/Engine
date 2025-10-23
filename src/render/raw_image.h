#pragma once

#include <vector>
#include <glad/glad.h>
#include "../bmp/bmp.h"

// Структуры для работы с цветом пикселя в формате RGB
#pragma pack(push)
#pragma pack(1)
union RGB {
  struct {
    uint8_t r; // Красный канал
    uint8_t g; // Зелёный канал
    uint8_t b; // Синий канал
  };
  uint8_t color[3] = {0}; // Альтернативный доступ к цветам как к массиву
};
#pragma pack(pop)

RGB sumRGB(const RGB& col1, const RGB& col2);
RGB operator*(const RGB& color, float brightness);
RGB operator*(float brightness, const RGB& color);

class RawImage {
public:
  RawImage(uint32_t width, uint32_t height);
  ~RawImage();

  // Загружает данные изображения из BMP-файла, используя структуры из bmp.h
  bool loadFromBMP(const std::string& filename);

  // Создаёт OpenGL-текстуру из данных изображения в raw_data_
  void createTexture();

  // Привязывает созданную текстуру к текущему контексту OpenGL
  void bindTexture() const;

  // Возвращает ID текстуры для использования в рендеринге
  GLuint getTextureID() const { return textureID; }

  // Устанавливает цвет пикселя по координатам (x, y)
  void SetPixel(uint32_t x, uint32_t y, const RGB &color);

  // Возвращает ширину изображения
  uint32_t GetWidth() const { return width_; }

  // Возвращает высоту изображения
  uint32_t GetHeight() const { return height_; }

  // Возвращает указатель на сырые данные изображения
  const uint8_t *raw_data() const { return raw_data_.data(); }

private:
  uint32_t width_; // Ширина изображения
  uint32_t height_; // Высота изображения
  std::vector<uint8_t> raw_data_; // Буфер для хранения пиксельных данных (BGR)
  GLuint textureID; // ID OpenGL-текстуры
};