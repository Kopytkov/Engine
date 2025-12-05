#include "camera_parser.h"
#include <array>

using json = nlohmann::json;

std::optional<Camera> CameraParser::Parse(const json& j) {
  const std::vector<std::string> required = {"position", "target", "resolution",
                                             "fov_deg"};
  for (const auto& f : required) {
    if (!j.contains(f)) {
      throw std::runtime_error("Missing camera." + f);
    }
  }

  auto pos_arr = parseVec3(j["position"], "camera.position");
  vec3 pos(pos_arr[0], pos_arr[1], pos_arr[2]);

  auto target_arr = parseVec3(j["target"], "camera.target");
  vec3 target(target_arr[0], target_arr[1], target_arr[2]);
  vec3 view = normalize(target - pos);

  std::array<float, 3> up_arr = {0, 1, 0};
  if (j.contains("up")) {
    up_arr = parseVec3(j["up"], "camera.up");
  }
  vec3 up(up_arr[0], up_arr[1], up_arr[2]);

  auto res = j["resolution"].get<std::array<int, 2>>();
  if (res[0] <= 0 || res[1] <= 0) {
    throw std::runtime_error("Invalid resolution");
  }

  float pix = j.value("pixel_size_mm", 0.26f);
  float fov = j["fov_deg"].get<float>();

  return Camera(pos, view, up, res[0], res[1], pix, fov);
}
