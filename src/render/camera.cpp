#include "camera.h"
#include <cmath>
#include "math/vec_functions.h"

Camera::Camera(const vec3& pos,
               const vec3& view,
               vec3 up,
               int w,
               int h,
               float pixInMm,
               float FOV)
    : position(pos),
      viewVec(normalize(view)),
      wResolution(w),
      hResolution(h),
      pixInMillimeter(pixInMm),
      fovDeg(FOV) {
  cameraUp = normalize(up - viewVec * dot(up, viewVec));
  RecalculateScreenDist();
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

void Camera::SetPosition(const vec3& newPos) {
  position = newPos;
}

// Смещение в мировых координатах
void Camera::Move(const vec3& delta) {
  position += delta;
}

// Поворот камеры: yaw вокруг глобальной Y, pitch вокруг локальной X
void Camera::Rotate(float yawDegrees, float pitchDegrees) {
  // Yaw — влево/вправо, Pitch — вверх/вниз
  float yawRad = yawDegrees * M_PI / 180.0f;
  float pitchRad = pitchDegrees * M_PI / 180.0f;

  vec3 right = normalize(cross(viewVec, cameraUp));

  // Yaw
  viewVec = normalize(viewVec * std::cos(yawRad) + right * std::sin(yawRad));

  // Pitch
  viewVec =
      normalize(viewVec * std::cos(pitchRad) + cameraUp * std::sin(pitchRad));

  // Обновляем вектор вверх
  cameraUp = normalize(cross(right, viewVec));
}

// Пересчёт screenDist по FOV и разрешению
void Camera::RecalculateScreenDist() {
  float halfFovRad = fovDeg * M_PI / 360.0f;
  float halfScreenWidthMm = float(wResolution) / 2.0f * pixInMillimeter;

  screenDist = halfScreenWidthMm / std::tan(halfFovRad);
}

// Меняем разрешение и пересчитываем экран
void Camera::SetResolution(int w, int h) {
  wResolution = w;
  hResolution = h;
  RecalculateScreenDist();
}
