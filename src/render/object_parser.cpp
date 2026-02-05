#include "object_parser.h"

using json = nlohmann::json;

// Вспомогательная функция для парсинга физического тела
static std::unique_ptr<PhysicsBody> ParsePhysicsBody(const json& j,
                                                     const vec3& initialPos) {
  auto body = std::make_unique<PhysicsBody>();
  body->SetPosition(initialPos);

  if (j.contains("mass")) {
    float m = j["mass"].get<float>();
    body->SetMass(m);
  }

  // Добавляем парсинг затухания (для будущего)
  if (j.contains("linear_damping")) {
    body->linearDamping = j["linear_damping"].get<float>();
  }
  if (j.contains("angular_damping")) {
    body->angularDamping = j["angular_damping"].get<float>();
  }

  if (j.contains("physics_material")) {
    const auto& physMat = j["physics_material"];
    if (physMat.contains("friction")) {
      body->material->friction = physMat["friction"].get<float>();
    }
    if (physMat.contains("restitution")) {
      body->material->restitution = physMat["restitution"].get<float>();
    }
  }
  return body;
}

std::unique_ptr<SceneEntity> ObjectParser::Parse(const json& j) {
  if (!j.contains("type")) {
    throw std::runtime_error("Missing object.type");
  }
  if (!j.contains("position")) {
    throw std::runtime_error("Missing object.position");
  }

  std::string type = j["type"].get<std::string>();
  vec3 pos = vec3(parseVec3(j["position"], "object.position"));

  std::unique_ptr<SceneObject> sceneObject = nullptr;

  // Переменные для хранения размеров, чтобы передать их в физику
  float sphereRadius = 0.0f;
  vec3 boxHalfExtents{0.0f};

  // Парсим sphere
  if (type == "sphere") {
    if (!j.contains("radius")) {
      throw std::runtime_error("Missing object.radius");
    }
    float r = j["radius"].get<float>();
    if (r <= 0) {
      throw std::runtime_error("Invalid radius");
    }
    sphereRadius = r;

    auto mat_ptr = j.contains("material")
                       ? parseMaterial(j["material"])
                       : parseMaterial(json{{"type", "pbr"}});

    sceneObject = std::make_unique<Sphere>(pos, r, std::move(mat_ptr));

  }
  // Парсим box
  else if (type == "box") {
    if (!j.contains("vertex")) {
      throw std::runtime_error("Missing object.vertex for box");
    }

    vec3 vertex = vec3(parseVec3(j["vertex"], "object.vertex"));
    boxHalfExtents = vertex;

    auto mat_ptr = j.contains("material")
                       ? parseMaterial(j["material"])
                       : parseMaterial(json{{"type", "pbr"}});

    sceneObject = std::make_unique<Box>(pos, vertex, std::move(mat_ptr));
  } else {
    throw std::runtime_error("Unknown object type: " + type);
  }

  // Создаем физическое тело
  auto physicsBody = ParsePhysicsBody(j, pos);

  // Если тело динамическое (mass > 0), настраиваем тензор инерции
  if (!physicsBody->isStatic) {
    if (type == "sphere") {
      physicsBody->SetInertiaSphere(sphereRadius);
    } else if (type == "box") {
      physicsBody->SetInertiaBox(boxHalfExtents);
    }
  }

  // Собираем сущность
  return std::make_unique<SceneEntity>(std::move(sceneObject),
                                       std::move(physicsBody));
}
