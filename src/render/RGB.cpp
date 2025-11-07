#include "RGB.h"
#include <algorithm>

RGB RGB::multiplyColors(const RGB& other) const {
  return RGB{static_cast<uint8_t>(
                 (static_cast<int>(r) * static_cast<int>(other.r)) / 255),
             static_cast<uint8_t>(
                 (static_cast<int>(g) * static_cast<int>(other.g)) / 255),
             static_cast<uint8_t>(
                 (static_cast<int>(b) * static_cast<int>(other.b)) / 255)};
}

RGB stretchRGB(const RGB& col1, const RGB& col2) {
  RGB colRes{0, 0, 0};
  colRes.r = std::clamp(col1.r + col2.r, 0, 255);
  colRes.g = std::clamp(col1.g + col2.g, 0, 255);
  colRes.b = std::clamp(col1.b + col2.b, 0, 255);
  return colRes;
}

RGB clampRGB(const RGB& color, const RGB& light) {
  return RGB{std::min(color.r, light.r), std::min(color.g, light.g),
             std::min(color.b, light.b)};
}

RGB operator*(const RGB& color, float brightness) {
  RGB colRes;
  colRes.r = static_cast<uint8_t>(color.r * std::clamp(brightness, 0.0f, 1.0f));
  colRes.g = static_cast<uint8_t>(color.g * std::clamp(brightness, 0.0f, 1.0f));
  colRes.b = static_cast<uint8_t>(color.b * std::clamp(brightness, 0.0f, 1.0f));
  return colRes;
}

RGB operator*(float brightness, const RGB& color) {
  return color * brightness;
}
