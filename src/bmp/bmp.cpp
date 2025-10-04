#include "bmp.h"

#include "render/raw_image.h"

BMP::BMP(const RawImage &raw) : raw_data_(raw) {
  info_.imageInfo.fWidth = raw.GetWidth();
  info_.imageInfo.fHeight = raw.GetHeight();
  info_.fileInfo.fDataOffset = sizeof(info_.fileInfo) + sizeof(info_.imageInfo);

  info_.imageInfo.fSizeImage =
      ((info_.imageInfo.fWidth * info_.imageInfo.fBitCount + 31) / 32) * 4 *
      info_.imageInfo.fHeight;

  info_.fileInfo.fSize =
      info_.fileInfo.fDataOffset + info_.imageInfo.fSizeImage;
}

const bmpInfo &BMP::getInfo() const { return info_; }

void BMP::Write(std::ofstream &outfile) const {
  outfile.write(reinterpret_cast<const char *>(&info_), sizeof(bmpInfo));
  outfile.write(reinterpret_cast<const char *>(raw_data_.raw_data()),
                info_.imageInfo.fSizeImage);
}
