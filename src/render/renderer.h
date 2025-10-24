#pragma once

#include "camera.h"
#include "raw_image.h"
#include "ray.h"
#include "scene.h"

class Renderer {
 public:
  void Render(const Camera& camera, const Scene& scene, RawImage& out_image);

 private:
};
