#include "bmp.h"
#include <iostream>

#include "render/raw_image.h"

BMP::BMP(const RawImage &raw){
  info_.imageInfo.fWidth = raw.GetWidth();
  info_.imageInfo.fHeight = raw.GetHeight();
  info_.fileInfo.fDataOffset = sizeof(info_.fileInfo) + sizeof(info_.imageInfo);

  info_.imageInfo.fSizeImage =
      ((info_.imageInfo.fWidth * info_.imageInfo.fBitCount + 31) / 32) * 4 *
      info_.imageInfo.fHeight;

  info_.fileInfo.fSize =
      info_.fileInfo.fDataOffset + info_.imageInfo.fSizeImage;

  dib.resize(info_.imageInfo.fSizeImage);

  const uint32_t line_bytes = info_.imageInfo.fSizeImage / info_.imageInfo.fHeight;
  for(uint32_t x = 0; x < info_.imageInfo.fWidth; x++){
    for(uint32_t y = 0; y < info_.imageInfo.fHeight; y++){
        dib[sizeof(RGB) * x + line_bytes * y + 0] = raw.raw_data()[sizeof(RGB) * x + y * info_.imageInfo.fWidth * sizeof(RGB) + 0];
        dib[sizeof(RGB) * x + line_bytes * y + 1] = raw.raw_data()[sizeof(RGB) * x + y * info_.imageInfo.fWidth * sizeof(RGB) + 1];
        dib[sizeof(RGB) * x + line_bytes * y + 2] = raw.raw_data()[sizeof(RGB) * x + y * info_.imageInfo.fWidth * sizeof(RGB) + 2];
    }
  }
}

const bmpInfo &BMP::getInfo() const { return info_; }

void BMP::Write(std::filesystem::path& out) const {

  std::ofstream outfile(out, std::ios_base::binary);

  outfile.write((const char*)(&info_), sizeof(bmpInfo));
  outfile.write((const char*)(dib.data()), dib.size());
}