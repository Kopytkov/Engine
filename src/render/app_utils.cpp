#include "app_utils.h"
#include <iostream>
#include <vector>

#include "bmp/bmp.h"
#include "material_pbr.h"
#include "math/vec.h"
#include "scene_object_sphere.h"
#include "texture.h"
#include "texture_manager.h"

bool AppUtils::InitRaymarchBallResources(
    const Scene& scene,
    Shader& raymarchShader,
    std::vector<Texture>& outTextures,
    std::vector<BallMaterialGPU>& outMaterials) {
  outTextures.clear();
  outMaterials.clear();

  // Собираем объекты-сферы
  const auto& objects = scene.GetObjects();
  std::vector<const Sphere*> spheres;
  for (const auto& obj : objects) {
    if (auto* sphere = dynamic_cast<const Sphere*>(obj.get())) {
      spheres.push_back(sphere);
    }
  }

  size_t sphere_count = spheres.size();
  if (sphere_count == 0) {
    return true;
  }

  // Ограничиваем количество сфер и выделяем память
  if (sphere_count > 16) {
    sphere_count = 16;
  }
  outMaterials.resize(sphere_count);

  // Конвертируем C++ материалы в GPU-совместимый формат
  for (size_t i = 0; i < sphere_count; ++i) {
    const Sphere* sphere = spheres[i];
    const Material& generic_mat = sphere->GetMaterial();
    const MaterialPBR* pbr_mat = dynamic_cast<const MaterialPBR*>(&generic_mat);

    BallMaterialGPU& gpu_mat = outMaterials[i];
    gpu_mat.textureID = -1;

    if (pbr_mat) {
      // Копируем свойства материала из C++ объекта в GPU-структуру
      gpu_mat.baseColor = RGBtoVec3(pbr_mat->getBaseColor());
      gpu_mat.roughness = pbr_mat->getRoughness();
      gpu_mat.metallic = pbr_mat->getMetallic();
      gpu_mat.transmission = pbr_mat->getTransmission();
      gpu_mat.refraction = pbr_mat->getRefraction();

      const std::string& texName = pbr_mat->getTextureName();
      if (!texName.empty()) {
        try {
          // Загружаем BMP, создаем текстуру OpenGL и сохраняем ее индекс
          RawImage img = loadFromBMP(texName);
          if (img.GetWidth() > 0) {
            outTextures.emplace_back(img);
            outTextures.back().createTexture();
            gpu_mat.textureID = static_cast<int>(outTextures.size() - 1);
          } else {
            std::cerr << "Warning: Loaded empty image for texture '" << texName
                      << "'\n";
          }
        } catch (const std::exception& e) {
          std::cerr << "Texture error for '" << texName << "': " << e.what()
                    << "\n";
        }
      }
    } else {
      // Если материал объекта не PBR, используем отладочный цвет
      gpu_mat.baseColor = vec3(1.0, 0.0, 1.0);
      gpu_mat.roughness = 0.5f;
      gpu_mat.metallic = 0.0f;
      gpu_mat.transmission = 0.0f;
      gpu_mat.refraction = 1.5f;
    }
  }

  // Единоразово загружаем статичные данные в uniform-переменные шейдера
  raymarchShader.use();

  // Загружаем массив структур с материалами
  for (size_t i = 0; i < sphere_count; ++i) {
    std::string idx = "[" + std::to_string(i) + "]";
    const auto& mat = outMaterials[i];

    raymarchShader.setVec3("ballMaterials" + idx + ".baseColor", mat.baseColor);
    raymarchShader.setFloat("ballMaterials" + idx + ".roughness",
                            mat.roughness);
    raymarchShader.setFloat("ballMaterials" + idx + ".metallic", mat.metallic);
    raymarchShader.setFloat("ballMaterials" + idx + ".transmission",
                            mat.transmission);
    raymarchShader.setFloat("ballMaterials" + idx + ".refraction",
                            mat.refraction);
    raymarchShader.setInt("ballMaterials" + idx + ".textureID", mat.textureID);
  }

  // Загружаем глобальные параметры, общие для всех шаров
  raymarchShader.setInt("ballCount", static_cast<int>(sphere_count));
  raymarchShader.setFloat("ballRadius", 1.35f);

  return true;
}

void AppUtils::BindBallTextures(const std::vector<Texture>& textures,
                                Shader& shader) {
  shader.use();

  for (size_t i = 0; i < textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE1 + static_cast<GLenum>(i));
    textures[i].bindTexture();

    // Явно указываем шейдеру номер текстурного юнита
    std::string uniformName = "ballTextures[" + std::to_string(i) + "]";
    shader.setInt(uniformName, 1 + static_cast<int>(i));
  }
}
