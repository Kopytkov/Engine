#include "raw_image.h"

RawImage::RawImage(uint32_t width, uint32_t height)
    : width_(width), height_(height),
      raw_data_(width_ * height_ * sizeof(RGB), 0) {}

void RawImage::SetPixel(uint32_t x, uint32_t y, const RGB &color) {
  raw_data_[sizeof(RGB) * x + y + 0] = color.b;
  raw_data_[sizeof(RGB) * x + y + 1] = color.g;
  raw_data_[sizeof(RGB) * x + y + 2] = color.r;
}

uint32_t RawImage::GetWidth() const { return width_; }

uint32_t RawImage::GetHeight() const { return height_; }

const uint8_t *RawImage::raw_data() const { return raw_data_.data(); }