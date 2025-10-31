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
  cameraUp = normalize(up - viewVec * dot(up, viewVec));
  wResolution = w;
  hResolution = h;
  pixInMillimeter = pixInMm;
  screenDist =
      1 / std::tan(FOV * 3.14159265f / 360) / pixInMm * float(w) / 2.0f;
}

Camera::~Camera() = default;

std::tuple<uint32_t, uint32_t, Ray> Camera::GetRay(uint32_t x,
                                                   uint32_t y) const {
  vec3 startPixel =
      position + screenDist * viewVec -
      cameraUp / pixInMillimeter * (float(hResolution) / 2.0f - 0.5f) /**/
      - cross(viewVec, cameraUp) / pixInMillimeter *
            (float(wResolution) / 2.0f - 0.5f);

  const Ray ray(startPixel + x / pixInMillimeter * cross(viewVec, cameraUp) +
                    y / pixInMillimeter * cameraUp,
                startPixel + x / pixInMillimeter * cross(viewVec, cameraUp) +
                    y / pixInMillimeter * cameraUp - position,
                0);

  return {x, y, ray};
}
