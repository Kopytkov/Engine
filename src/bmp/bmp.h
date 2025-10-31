#pragma once

#include <fstream>
#include <vector>
#include <filesystem>

#pragma pack(push, 1)
struct bfi {
  uint16_t fSignature = 19778;
  uint32_t fSize = 0;
  uint32_t fReserved = 0;
  uint32_t fDataOffset = 0;
};

struct cfi {
  uint32_t fSize = 40;
  uint32_t fWidth = 0;
  uint32_t fHeight = 0;
  uint16_t fPlanes = 1;
  uint16_t fBitCount = 24;
  uint32_t fCompression = 0;
  uint32_t fSizeImage = 0;
  uint32_t XpixelsPerM = 0;
  uint32_t YpixelsPerM = 0;
  uint32_t ColorsUsed = 0;
  uint32_t ColorsImportant = 0;
};

struct bmpInfo {
  bfi fileInfo;
  cfi imageInfo;
};
#pragma pack(pop)

class RawImage;

class BMP {
 public:
  BMP(const RawImage& raw);

  const bmpInfo& getInfo() const;

  void Write(std::filesystem::path& out) const;

 private:
  bmpInfo info_;
  // const RawImage& raw_data_;
  std::vector<uint8_t> dib;
};
