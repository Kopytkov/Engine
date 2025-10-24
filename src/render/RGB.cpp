#include "RGB.h"
#include <iostream>

RGB sumRGB(const RGB& col1, const RGB& col2) {
  RGB colRes{0, 0, 0};
  colRes.r = std::max(0, std::min(255, col1.r + col2.r));
  colRes.g = std::max(0, std::min(255, col1.g + col2.g));
  colRes.b = std::max(0, std::min(255, col1.b + col2.b));
  return colRes;
}

RGB cutRGB(const RGB& color, const RGB& light) {
  return RGB{std::min(color.r, light.r), std::min(color.g, light.g),
             std::min(color.b, light.b)};
}

RGB operator*(const RGB& color, float brightness) {
  RGB colRes;
  colRes.r = static_cast<uint8_t>(color.r *
                                  std::max(0.0f, std::min(brightness, 1.0f)));
  colRes.g = static_cast<uint8_t>(color.g *
                                  std::max(0.0f, std::min(brightness, 1.0f)));
  colRes.b = static_cast<uint8_t>(color.b *
                                  std::max(0.0f, std::min(brightness, 1.0f)));
  return colRes;
}

RGB operator*(float brightness, const RGB& color) {
  RGB colRes;
  colRes.r = static_cast<uint8_t>(color.r *
                                  std::max(0.0f, std::min(brightness, 1.0f)));
  colRes.g = static_cast<uint8_t>(color.g *
                                  std::max(0.0f, std::min(brightness, 1.0f)));
  colRes.b = static_cast<uint8_t>(color.b *
                                  std::max(0.0f, std::min(brightness, 1.0f)));
  return colRes;
}