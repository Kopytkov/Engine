#include "camera.h"

#include "math/vec_functions.h"

Camera::Camera(const vec3& pos,
               const vec3& view,
               vec3 up,
               int w,
               int h,
               float pixInMm,
               float FOV) {
  position = pos;
  viewVec = normalize(view);
  cameraUp = normalize(up - view * dot(up, view));
  wResolution = w;
  hResolution = h;
  pixInMillimeter = pixInMm;
  screenDist =
      1 / std::tan(FOV * 3.14159265f / 360) / pixInMm * float(w) / 2.0f;
}

Camera::~Camera() = default;

std::tuple<uint32_t, uint32_t, Ray> Camera::GetRay(uint32_t x,
                                                   uint32_t y) const {
  // Центр плоскости экрана в мире
  vec3 screenCenter = position + screenDist * viewVec;

  // Смещение от центра к левому-верхнему углу (половина экрана в мировых
  // единицах)
  vec3 topLeftOffset =
      -cameraUp * (float(hResolution) / 2.0f) / pixInMillimeter -
      cross(viewVec, cameraUp) * (float(wResolution) / 2.0f) / pixInMillimeter;

  // Позиция левого-верхнего пикселя
  vec3 topLeftPixel = screenCenter + topLeftOffset;

  // Позиция текущего пикселя (через центр, +0.5f)
  vec3 pixelWorldPos = topLeftPixel +
                       (x + 0.5f) / pixInMillimeter * cross(viewVec, cameraUp) +
                       (y + 0.5f) / pixInMillimeter * cameraUp;

  // Луч от камеры через пиксель (раньше шёл от пикселя к камере)
  const Ray ray(position, pixelWorldPos - position, 0);

  return {x, y, ray};
}
