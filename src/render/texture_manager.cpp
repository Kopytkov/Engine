#include "texture_manager.h"
#include <fstream>
#include <stdexcept>

TextureManager& TextureManager::GetInstance() {
  static TextureManager instance;
  return instance;
}

void TextureManager::Initialize() {
  // Получаем список всех ожидаемых имён объектов из sphere.json
  std::ifstream sphere_file(sphere_json_path_);
  if (!sphere_file.is_open()) {
    throw std::runtime_error("Cannot open sphere.json");
  }
  json sphere_data = json::parse(sphere_file);

  std::vector<std::string> expected_names;
  for (const auto& item : sphere_data) {
    if (item.contains("name") && item["name"].is_string()) {
      expected_names.emplace_back(item["name"].get<std::string>());
    }
  }

  bool need_generate = CheckNeedsGeneration(expected_names);

  // Если чего-то не хватает — запускаем Python-скрипт
  if (need_generate) {
    GenerateTextures();
  }

  // Загружаем манифест после возможной генерации
  LoadManifest();
}

std::string TextureManager::GetTexturePath(const std::string& name) const {
  auto it = texture_paths_.find(name);
  if (it != texture_paths_.end()) {
    return textures_dir_ + "/" + it->second;
  }
  throw std::runtime_error("Texture not found for name: " + name);
}

void TextureManager::LoadManifest() {
  texture_paths_.clear();

  if (!std::filesystem::exists(manifest_path_)) {
    throw std::runtime_error("Manifest file does not exist: " + manifest_path_);
  }

  std::ifstream mf(manifest_path_);
  if (!mf) {
    throw std::runtime_error("Cannot open manifest file: " + manifest_path_);
  }

  json manifest;
  mf >> manifest;

  for (const auto& [key, value] : manifest.items()) {
    if (value.is_string()) {
      texture_paths_[key] = value.get<std::string>();
    }
  }
}

bool TextureManager::CheckNeedsGeneration(
    const std::vector<std::string>& expected_names) {
  bool need_generate = false;

  // Загружаем манифест (если есть)
  json manifest = json::object();  // пустой объект по умолчанию
  if (std::filesystem::exists(manifest_path_)) {
    std::ifstream mf(manifest_path_);
    if (mf) {
      mf >> manifest;
    }
  } else {
    need_generate = true;  // манифеста нет — точно нужно генерировать
  }

  // Проверяем каждую текстуру
  if (!need_generate) {
    for (const auto& name : expected_names) {
      if (!manifest.contains(name)) {
        need_generate = true;
        break;
      }

      std::string rel_path = manifest[name];
      std::filesystem::path full_path =
          std::filesystem::path(textures_dir_) / rel_path;

      if (!std::filesystem::exists(full_path)) {
        need_generate = true;
        break;
      }
    }
  }

  return need_generate;
}

void TextureManager::GenerateTextures() {
  int result = std::system(python_script_cmd_.c_str());
  if (result != 0) {
    throw std::runtime_error("Python script failed (code " +
                             std::to_string(result) + ")");
  }
}
