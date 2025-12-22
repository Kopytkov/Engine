#include "app_utils.h"
#include <fstream>
#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>
#include "bmp/bmp.h"
#include "math/vec.h"
#include "raw_image.h"
#include "texture.h"
#include "texture_manager.h"

using json = nlohmann::json;

// Структура, повторяющая layout в шейдере
struct BallMaterialGPU {
  vec3 baseColor;
  float roughness;
  float metallic;
  float transmission;
  float refraction;
  int textureID;
  float padding[2];
};

bool AppUtils::SetupRaymarchBallRendering(Shader& raymarchShader) {
  static std::vector<Texture> ballTextures;
  static std::vector<BallMaterialGPU> ballMaterialsGPU;

  // Очистка на случай повторного вызова
  ballTextures.clear();
  ballMaterialsGPU.clear();

  // Открываем JSON-файл с описанием всех шаров
  std::ifstream sphereFile("assets/scene/objects/sphere.json");
  if (!sphereFile.is_open()) {
    std::cerr << "Cannot open sphere.json" << std::endl;
    return false;
  }

  json sphereJson;
  sphereFile >> sphereJson;

  // Определяем количество шаров
  const size_t numBalls = sphereJson.size();

  if (numBalls == 0) {
    std::cerr << "No balls defined in sphere.json" << std::endl;
    return false;
  }

  // Защита от превышения лимита массива в шейдере
  if (numBalls > 64) {
    std::cerr << "Too many balls (" << numBalls << "), maximum supported: 64"
              << std::endl;
    return false;
  }

  // Резервируем память заранее для производительности
  ballTextures.reserve(numBalls);
  ballMaterialsGPU.resize(numBalls);

  // Основной цикл: обработка каждого шара из JSON
  for (size_t i = 0; i < numBalls; ++i) {
    const auto& ball = sphereJson[i];
    std::string name = ball["name"].get<std::string>();
    const auto& matJson = ball["material"];

    BallMaterialGPU& mat = ballMaterialsGPU[i];

    // Базовые параметры материала
    mat.baseColor =
        vec3(matJson["color"][0].get<float>(), matJson["color"][1].get<float>(),
             matJson["color"][2].get<float>());
    mat.roughness = matJson.value("roughness", 0.2f);
    mat.metallic = matJson.value("metallic", 0.0f);
    mat.transmission = matJson.value("transparency", 0.0f);
    mat.refraction = matJson.value("refraction", 1.5f);
    mat.textureID = -1;  // -1 означает "без текстуры"
    mat.padding[0] = mat.padding[1] = 0.0f;

    // Если у шара указана текстура — загружаем её
    if (matJson.contains("texture")) {
      std::string texPath = TextureManager::GetInstance().GetTexturePath(name);
      RawImage texImage = loadFromBMP(texPath);

      if (texImage.GetWidth() == 0 || texImage.GetHeight() == 0) {
        std::cerr << "Failed to load texture for ball '" << name
                  << "': " << texPath << std::endl;
        continue;
      }

      ballTextures.emplace_back(texImage);
      ballTextures.back().createTexture();  // Генерация GL-текстуры
      mat.textureID = static_cast<int>(ballTextures.size() - 1);
    }
  }

  // Передаём материалы в шейдер
  raymarchShader.use();

  for (size_t i = 0; i < numBalls; ++i) {
    std::string idx = "[" + std::to_string(i) + "]";
    const BallMaterialGPU& mat = ballMaterialsGPU[i];

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

  // Привязываем загруженные текстуры
  for (size_t i = 0; i < ballTextures.size(); ++i) {
    glActiveTexture(GL_TEXTURE1 + static_cast<GLenum>(i));
    ballTextures[i].bindTexture();
    raymarchShader.setInt("ballTextures[" + std::to_string(i) + "]",
                          1 + static_cast<int>(i));
  }

  // Передаём позиции шаров из JSON в uniform-массив шейдера
  for (size_t i = 0; i < numBalls; ++i) {
    const auto& ball = sphereJson[i];
    const auto& posJson = ball["position"];
    vec3 pos(posJson[0].get<float>(), posJson[1].get<float>(),
             posJson[2].get<float>());

    std::string idx = "[" + std::to_string(i) + "]";
    raymarchShader.setVec3("ballPositions" + idx, pos);
  }

  // Передаём количество шаров в шейдер
  raymarchShader.setInt("ballCount", static_cast<int>(numBalls));

  // Радиус шаров
  raymarchShader.setFloat("ballRadius", 1.35f);

  return true;
}
