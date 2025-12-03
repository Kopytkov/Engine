#include "raw_image.h"
#include <fstream>
#include <iostream>

// Загрузка BMP-файла с использованием структур из bmp.h
RawImage loadFromBMP(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open BMP file: " << filename << std::endl;
    return RawImage(0, 0);
  }

  // Чтение заголовка BMP (bfi и cfi из bmp.h)
  bmpInfo info;
  file.read(reinterpret_cast<char*>(&info), sizeof(bmpInfo));
  if (info.fileInfo.fSignature != 19778) {  // Проверка сигнатуры 'BM'
    std::cerr << "Invalid BMP signature" << std::endl;
    file.close();
    return RawImage(0, 0);
  }

  // Создание объекта RawImage с нужными размерами
  RawImage image(info.imageInfo.fWidth, info.imageInfo.fHeight);
  uint32_t line_bytes = ((image.GetWidth() * 24 + 31) / 32) * 4;

  // Чтение пиксельных данных
  file.seekg(info.fileInfo.fDataOffset);
  file.read(reinterpret_cast<char*>(const_cast<uint8_t*>(image.raw_data())),
            line_bytes * image.GetHeight());
  file.close();
  return image;
}

RawImage::RawImage(uint32_t width, uint32_t height)
    : width_(width),
      height_(height),
      raw_data_(((width * 24 + 31) / 32) * 4 * height, 125) {}

void RawImage::SetPixel(uint32_t x, uint32_t y, const RGB& color) {
  const uint32_t line_bytes = ((width_ * 24 + 31) / 32) * 4;
  raw_data_[sizeof(RGB) * x + line_bytes * y + 0] = color.b;
  raw_data_[sizeof(RGB) * x + line_bytes * y + 1] = color.g;
  raw_data_[sizeof(RGB) * x + line_bytes * y + 2] = color.r;
}

RGB RawImage::GetPixel(uint32_t x, uint32_t y) const {
  // Защищаемся от выхода за границы
  if (x >= width_ || y >= height_) {
    return RGB{0, 0, 0};
  }

  const uint32_t line_bytes = ((width_ * 24 + 31) / 32) * 4;
  uint32_t idx = sizeof(RGB) * x + line_bytes * y;

  uint8_t b = raw_data_[idx + 0];
  uint8_t g = raw_data_[idx + 1];
  uint8_t r = raw_data_[idx + 2];

  return RGB{r, g, b};
}
