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

  // Геттеры
  const vec3& GetPosition() const { return position; }
  const vec3& GetViewVec() const { return viewVec; }
  const vec3& GetUpVec() const { return cameraUp; }
  int GetWidth() const { return wResolution; }
  int GetHeight() const { return hResolution; }
  float GetFOV() const { return fovDeg; }

  // Мутаторы
  void SetPosition(const vec3& newPos);
  void Move(const vec3& delta);
  void Rotate(float yawDegrees, float pitchDegrees);
  void SetResolution(int w, int h);

 private:
  vec3 position;

  vec3 viewVec;
  vec3 cameraUp;

  int wResolution;
  int hResolution;

  float pixInMillimeter;

  float screenDist;

  float fovDeg;

  void RecalculateScreenDist();
};
