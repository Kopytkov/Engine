#pragma once

#include <tuple>

#include "math/vec.h"
#include "ray.h"

class Camera {
 public:
  Camera(const vec3& pos,
         const vec3& view,
         vec3 up,
         int w,
         int h,
         float pixInMm,
         float FOV);
  ~Camera();

  std::tuple<uint32_t, uint32_t, Ray> GetRay(uint32_t x, uint32_t y) const;

 private:
  vec3 position;

  vec3 viewVec;
  vec3 cameraUp;

  int wResolution;
  int hResolution;

  float pixInMillimeter;

  float screenDist;
};
