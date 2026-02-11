#include "texture_manager.h"
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include "raw_image.h"

TextureManager& TextureManager::GetInstance() {
  static TextureManager instance;
  return instance;
}

void TextureManager::Initialize() {
  // Получаем список всех ожидаемых имён объектов из sphere.json
  std::ifstream sphere_file(sphere_json_path_);
  if (!sphere_file.is_open()) {
    std::cerr << "Warning: Cannot open sphere.json for texture check."
              << std::endl;
  } else {
    json sphere_data = json::parse(sphere_file);
    std::vector<std::string> expected_names;
    for (const auto& item : sphere_data) {
      if (item.contains("name") && item["name"].is_string()) {
        expected_names.emplace_back(item["name"].get<std::string>());
      }
    }
    // Если чего-то не хватает — запускаем Python-скрипт
    if (CheckNeedsGeneration(expected_names)) {
      GenerateTextures();
    }
  }

  // Загружаем манифест после возможной генерации
  LoadManifest();
}

Texture* TextureManager::GetTexture(const std::string& name) {
  if (name.empty()) {
    return nullptr;
  }

  // Проверяем кэш уже загруженных текстур
  if (loaded_textures_.count(name)) {
    return loaded_textures_[name].get();
  }

  std::string cleanName = std::filesystem::path(name).filename().string();
  if (loaded_textures_.count(cleanName)) {
    return loaded_textures_[cleanName].get();
  }

  // Локальный кэш ошибок (чтобы не спамить консоль повторяющимися сообщениями)
  static std::set<std::string> reported_missing;
  if (reported_missing.count(name) || reported_missing.count(cleanName)) {
    return nullptr;
  }

  // Определяем реальный путь к файлу
  std::string finalPath;

  // Ищем в манифесте по короткому имени
  auto itManifest = texture_paths_.find(cleanName);
  if (itManifest != texture_paths_.end()) {
    finalPath = itManifest->second;
  }
  // Проверяем прямой путь
  else if (std::filesystem::exists(name)) {
    finalPath = name;
  }
  // Проверяем путь относительно папки текстур
  else {
    std::filesystem::path p = std::filesystem::path(textures_dir_) / cleanName;
    if (std::filesystem::exists(p)) {
      finalPath = p.string();
    }
  }

  if (finalPath.empty()) {
    std::cerr << "TextureManager Error: Could not find file for '" << name
              << "'" << std::endl;
    reported_missing.insert(name);  // Запоминаем ошибку
    return nullptr;
  }

  // Загружаем изображение и создаём текстуру
  try {
    RawImage img = loadFromBMP(finalPath);

    auto newTexture = std::make_unique<Texture>(img);
    newTexture->createTexture();

    Texture* ptr = newTexture.get();

    // Кэшируем под исходным именем
    loaded_textures_[name] = std::move(newTexture);

    return ptr;
  } catch (const std::exception& e) {
    std::cerr << "TextureManager Error: Failed to load BMP '" << finalPath
              << "': " << e.what() << std::endl;
    reported_missing.insert(name);
    return nullptr;
  }
}

const std::string& TextureManager::GetTexturePath(
    const std::string& name) const {
  auto it = texture_paths_.find(name);
  if (it != texture_paths_.end()) {
    return it->second;
  }
  throw std::runtime_error("Texture not found for name: " + name);
}

void TextureManager::Shutdown() {
  loaded_textures_.clear();
  texture_paths_.clear();
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
      // Сохраняем полный путь
      texture_paths_[key] = textures_dir_ + "/" + value.get<std::string>();
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
