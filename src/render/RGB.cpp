#include "RGB.h"
#include <iostream>
#include <algorithm>

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