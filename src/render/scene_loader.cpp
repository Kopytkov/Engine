#include "scene_loader.h"
#include <fstream>

using json = nlohmann::json;

// Внутренняя функция: разрешение ref
static json resolveRef(const json& node, const std::string& baseDir) {
  if (node.is_object() && node.contains("ref")) {
    std::string refPath = node["ref"].get<std::string>();
    std::filesystem::path fullPath = std::filesystem::path(baseDir) / refPath;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
      throw std::runtime_error("Cannot open referenced file: " +
                               fullPath.string());
    }

    json included;
    try {
      file >> included;
    } catch (const json::parse_error& e) {
      throw std::runtime_error("JSON parse error in " + fullPath.string() +
                               ": " + e.what());
    }
    return included;
  }
  return node;
}

// Основная загрузка
SceneLoader SceneLoader::Load(const std::string& jsonPath) {
  std::ifstream file(jsonPath);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open JSON file: " + jsonPath);
  }

  json j;
  try {
    file >> j;
  } catch (const json::parse_error& e) {
    throw std::runtime_error("JSON parse error in " + jsonPath + ": " +
                             e.what());
  }

  // Разрешение ref
  std::string baseDir = std::filesystem::path(jsonPath).parent_path().string();
  json resolved;

  for (auto& [key, value] : j.items()) {
    if (value.is_object() && value.contains("ref")) {
      resolved[key] = resolveRef(value, baseDir);
    } else {
      resolved[key] = value;
    }
  }

  // Создание сцены
  Scene scene;
  std::optional<Camera> camera = std::nullopt;

  if (resolved.contains("camera")) {
    camera = CameraParser::Parse(resolved["camera"]);
  }
  if (resolved.contains("lights")) {
    for (const auto& l : resolved["lights"]) {
      scene.AddLight(LightParser::Parse(l));
    }
  }
  if (resolved.contains("objects")) {
    const auto& objectsNode = resolved["objects"];

    // Если objects — это массив с ref
    if (objectsNode.is_array()) {
      for (const auto& item : objectsNode) {
        json resolvedItem;
        if (item.is_object() && item.contains("ref")) {
          resolvedItem = resolveRef(item, baseDir);
        } else {
          resolvedItem = item;
        }

        // Теперь resolvedItem — либо массив объектов, либо один объект
        if (resolvedItem.is_array()) {
          for (const auto& obj : resolvedItem) {
            scene.AddObject(ObjectParser::Parse(obj));
          }
        } else {
          scene.AddObject(ObjectParser::Parse(resolvedItem));
        }
      }
    }
    // Если objects — один ref
    else if (objectsNode.is_object() && objectsNode.contains("ref")) {
      json included = resolveRef(objectsNode, baseDir);
      if (included.is_array()) {
        for (const auto& obj : included) {
          scene.AddObject(ObjectParser::Parse(obj));
        }
      } else {
        scene.AddObject(ObjectParser::Parse(included));
      }
    }
    // Если objects — массив объектов
    else if (objectsNode.is_array()) {
      for (const auto& obj : objectsNode) {
        scene.AddObject(ObjectParser::Parse(obj));
      }
    }
  }

  return SceneLoader(std::move(scene), std::move(camera));
}
