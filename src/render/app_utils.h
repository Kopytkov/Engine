#pragma once

#include <vector>
#include "gl/shader.h"
#include "scene.h"
#include "texture.h"

struct BallMaterialGPU {
  vec3 baseColor;
  float roughness;
  float metallic;
  float transmission;
  float refraction;
  int textureID;
  float padding[2];
};

class AppUtils {
 public:
  // Одноразовая инициализация ресурсов и статических uniform'ов
  static bool InitRaymarchBallResources(
      const Scene& scene,
      Shader& raymarchShader,
      std::vector<Texture>& outBallTextures,
      std::vector<BallMaterialGPU>& outMaterialsGPU);

  // Привязка всех текстур шаров к текстурным юнитам
  static void BindBallTextures(const std::vector<Texture>& textures,
                               Shader& shader);
};
