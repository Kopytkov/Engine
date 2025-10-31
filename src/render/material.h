#pragma once
#include <iostream>

#include "bmp/bmp.h"
#include "math/vec.h"

#include "texture.h"
// #include "Scene.h"
class Scene;

class Material {
  Texture texture;

 public:
  float reflectivity = 0;
  float transparency = 0;
  float refraction = 0;

 public:
  ~Material();
  Material();
  Material(Texture& tex);
  Material(float refl, float tr, float refract, Texture& tex);

  RGB color(const Scene& scene,
            vec3 position,
            vec3 direction,
            vec3 norm,
            int numOfStep,
            bool isOutside);
};
