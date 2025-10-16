#include "raw_image.h"
#include <iostream>

RawImage::RawImage(uint32_t width, uint32_t height)
    : width_(width), height_(height),
      // raw_data_(width_ * height_ * sizeof(RGB), 125) {}
      raw_data_(((width * 24 + 31) / 32) * 4 * height, 125) {}
      

void RawImage::SetPixel(uint32_t x, uint32_t y, const RGB &color) {
  const uint32_t line_bytes = ((width_ * 24 + 31) / 32) * 4;
  raw_data_[sizeof(RGB) * x + line_bytes * y + 0] = color.b;
  raw_data_[sizeof(RGB) * x + line_bytes * y + 1] = color.g;
  raw_data_[sizeof(RGB) * x + line_bytes * y + 2] = color.r;
}

uint32_t RawImage::GetWidth() const { return width_; }

uint32_t RawImage::GetHeight() const { return height_; }

const uint8_t *RawImage::raw_data() const { return raw_data_.data(); }