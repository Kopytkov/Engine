#include <filesystem>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class TextureManager {
 public:
  static TextureManager& GetInstance();
  void Initialize();
  std::string GetTexturePath(const std::string& name) const;

 private:
  TextureManager() = default;
  ~TextureManager() = default;

  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;

  // Загрузка манифеста
  void LoadManifest();

  // Проверка необходимости генерации
  bool CheckNeedsGeneration(const std::vector<std::string>& expected_names);

  // Запуск Python-скрипта для генерации
  void GenerateTextures();

  std::map<std::string, std::string> texture_paths_;  // Кэш путей из манифеста

  const std::string textures_dir_ = "assets/textures";
  const std::string sphere_json_path_ = "assets/scene/objects/sphere.json";
  const std::string manifest_path_ = "assets/textures/textures_manifest.json";
  const std::string python_script_cmd_ =
      "python assets/textures/script_for_textures.py";
};
