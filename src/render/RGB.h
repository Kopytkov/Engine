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

  RGB multiplyColors(const RGB& other) const;
};
#pragma pack(pop)

// Умножение цветов
inline RGB multiplyColors(const RGB& a, const RGB& b) {
  return a.multiplyColors(b);
}
RGB stretchRGB(const RGB& col1, const RGB& col2);
RGB operator*(const RGB& color, float brightness);
RGB operator*(float brightness, const RGB& color);
RGB clampRGB(const RGB& color, const RGB& light);
