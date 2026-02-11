#pragma once
#include "gl/shader.h"
#include "gpu_data.h"
#include "material_pbr.h"

class MaterialConverter {
 public:
  // Превращает PBR материал в GPU структуру и биндинг текстуры
  static GPUMaterial Convert(const MaterialPBR* material, int textureUnitIndex);

  // Инициализация сэмплеров в шейдере
  static void InitShaderSamplers(const Shader& shader);
};
