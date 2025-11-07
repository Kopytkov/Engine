#pragma once
#include <cstdint>

// Структуры для работы с цветом пикселя в формате RGB
#pragma pack(push)
#pragma pack(1)
union RGB {
  struct {
    uint8_t r;  // Красный канал
    uint8_t g;  // Зелёный канал
    uint8_t b;  // Синий канал
  };
  uint8_t color[3] = {0};  // Альтернативный доступ к цветам как к массиву

  RGB operator*(const RGB& other) const {
    return RGB{static_cast<uint8_t>(
                   (static_cast<int>(r) * static_cast<int>(other.r)) / 255),
               static_cast<uint8_t>(
                   (static_cast<int>(g) * static_cast<int>(other.g)) / 255),
               static_cast<uint8_t>(
                   (static_cast<int>(b) * static_cast<int>(other.b)) / 255)};
  }
};
#pragma pack(pop)

RGB stretchRGB(const RGB& col1, const RGB& col2);
RGB operator*(const RGB& color, float brightness);
RGB operator*(float brightness, const RGB& color);
RGB clampRGB(const RGB& color, const RGB& light);
