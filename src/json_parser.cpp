#include "json_parser.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Object parseObject(const std::string& file) {
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + file);
    }

    json j;
    ifs >> j;

    Object obj;
    obj.name = j["name"].get<std::string>();
    obj.pos = glm::vec3(
        j["position"][0].get<float>(),
        j["position"][1].get<float>(),
        j["position"][2].get<float>()
    );
    obj.scale = glm::vec3(
        j["scale"][0].get<float>(),
        j["scale"][1].get<float>(),
        j["scale"][2].get<float>()
    );
    obj.texture = j["texture"].get<std::string>();

    return obj;
}