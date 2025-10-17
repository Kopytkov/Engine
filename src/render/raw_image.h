#pragma once

#include <vector>

#pragma pack(push)
#pragma pack(1)
union RGB {
  struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };
  uint8_t color[3] = {0};
};
#pragma pack(pop)

RGB sumRGB(const RGB& col1, const RGB& col2);
RGB operator*(const RGB& color, float brightness);
RGB operator*(float brightness, const RGB& color);

class RawImage {
public:
  RawImage(uint32_t width, uint32_t height);

  void SetPixel(uint32_t x, uint32_t y, const RGB &color);

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;
  const uint8_t *raw_data() const;

private:
  uint32_t width_;
  uint32_t height_;
  std::vector<uint8_t> raw_data_;
};