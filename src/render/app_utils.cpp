#include "app_utils.h"
#include <fstream>
#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>
#include "bmp/bmp.h"
#include "math/vec.h"
#include "raw_image.h"
#include "scene_object_sphere.h"
#include "texture.h"
#include "texture_manager.h"

using json = nlohmann::json;

bool AppUtils::InitRaymarchBallResources(
    Shader& raymarchShader,
    std::vector<Texture>& outTextures,
    std::vector<BallMaterialGPU>& outMaterials) {
  outTextures.clear();
  outMaterials.clear();

  // Загружаем описание шаров при инициализации
  std::ifstream file("assets/scene/objects/sphere.json");
  if (!file.is_open()) {
    std::cerr << "Cannot open sphere.json\n";
    return false;
  }

  json j;
  file >> j;

  size_t n = j.size();
  if (n == 0) {
    std::cerr << "No spheres defined in sphere.json\n";
    return false;
  }
  if (n > 16) {
    std::cerr << "Too many spheres: " << n << " (max 16 in shader)\n";
    return false;
  }

  outTextures.reserve(n);
  outMaterials.resize(n);

  // Парсим каждый шар
  for (size_t i = 0; i < n; ++i) {
    const auto& entry = j[i];
    std::string name = entry.value("name", "");

    const auto& m = entry["material"];

    BallMaterialGPU& mat = outMaterials[i];
    mat.baseColor = vec3(m["color"][0], m["color"][1], m["color"][2]);
    mat.roughness = m.value("roughness", 0.2f);
    mat.metallic = m.value("metallic", 0.0f);
    mat.transmission = m.value("transparency", 0.0f);
    mat.refraction = m.value("refraction", 1.5f);
    mat.textureID = -1;
    mat.padding[0] = mat.padding[1] = 0.0f;

    // Загружаем текстуру только если указана и имя шара известно
    if (m.contains("texture") && !name.empty()) {
      try {
        std::string path = TextureManager::GetInstance().GetTexturePath(name);
        RawImage img = loadFromBMP(path);
        if (img.GetWidth() > 0 && img.GetHeight() > 0) {
          outTextures.emplace_back(img);
          outTextures.back().createTexture();
          mat.textureID = static_cast<int>(outTextures.size() - 1);
        } else {
          std::cerr << "Failed to load texture: " << path << "\n";
        }
      } catch (const std::exception& e) {
        std::cerr << "Texture error for '" << name << "': " << e.what() << "\n";
      }
    }
  }

  // Передаём неизменяемые uniform'ы
  raymarchShader.use();

  // Материалы шаров
  for (size_t i = 0; i < n; ++i) {
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

  // Статические параметры сцены
  raymarchShader.setInt("ballCount", static_cast<int>(n));
  raymarchShader.setFloat("ballRadius", 1.35f);

  return true;
}

void AppUtils::UpdateBallPositions(Shader& raymarchShader, const Scene& scene) {
  int count = 0;
  const auto& objects = scene.GetObjects();

  for (const auto& objPtr : objects) {
    if (count >= 16) {
      break;  // Максимум 16 шаров в шейдере
    }

    if (auto* sphere = dynamic_cast<Sphere*>(objPtr.get())) {
      std::string idx = "[" + std::to_string(count) + "]";
      vec3 pos = sphere->GetPosition();
      raymarchShader.setVec3("ballPositions" + idx, pos);
      count++;
    }
  }
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
