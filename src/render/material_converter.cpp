#include "material_converter.h"
#include "math/vec_functions.h"
#include "texture_manager.h"

GPUMaterial MaterialConverter::Convert(const MaterialPBR* mat,
                                       int textureUnitIndex) {
  GPUMaterial gpuMat;
  // Значения по умолчанию
  gpuMat.r = 1.0f;
  gpuMat.g = 0.0f;
  gpuMat.b = 1.0f;
  gpuMat.roughness = 0.5f;
  gpuMat.metallic = 0.0f;
  gpuMat.transmission = 0.0f;
  gpuMat.refraction = 1.0f;
  gpuMat.textureID = -1;

  if (mat) {
    // Конвертация цвета из формата RGB класс в GPU формат
    const auto& color = mat->getBaseColor();
    gpuMat.r = color.r / 255.0f;
    gpuMat.g = color.g / 255.0f;
    gpuMat.b = color.b / 255.0f;

    // Копирование физических параметров PBR
    gpuMat.roughness = mat->getRoughness();
    gpuMat.metallic = mat->getMetallic();
    gpuMat.transmission = mat->getTransmission();
    gpuMat.refraction = mat->getRefraction();

    // Обработка текстуры
    const std::string& texName = mat->getTextureName();
    if (!texName.empty()) {
      Texture* tex = TextureManager::GetInstance().GetTexture(texName);
      if (tex) {
        // Активируем соответствующий текстурный слот (Unit)
        glActiveTexture(GL_TEXTURE0 + textureUnitIndex);
        // Привязываем саму текстуру к этому слоту
        tex->bindTexture();
        // Записываем индекс слота в структуру, чтобы шейдер знал, откуда читать
        gpuMat.textureID = textureUnitIndex;
      }
    }
  }
  return gpuMat;
}

void MaterialConverter::InitShaderSamplers(const Shader& shader) {
  shader.use();
  // Инициализация сэмплеров для текстур шаров
  for (int i = 0; i < 16; ++i) {
    shader.setInt("ballTextures[" + std::to_string(i) + "]", i);
  }
}
