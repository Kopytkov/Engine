#include "scene_object_box.h"
#include <algorithm>
#include "math/vec_functions.h"
#include "physics/physics_body.h"
#include "scene_object_sphere.h"

Box::Box(const vec3& position,
         const vec3& vertex,
         std::unique_ptr<Material> mat)
    : vertex_(abs(vertex)), material_(std::move(mat)) {
  SetPosition(position);
}

float Box::SDF(const vec3& point) const {
  vec3 q = abs(point - GetPosition()) - vertex_;
  return length(max(q, 0.0f)) +
         std::min(std::max(q[0], std::max(q[1], q[2])), 0.0f);
}

vec3 Box::getNormal(const vec3& point) const {
  vec3 q = abs(point - GetPosition()) - vertex_;
  vec3 q1 = normalize(max(q, 0.0f) +
                      clamp(q, std::max(q[0], std::max(q[1], q[2])), 0.0f));
  vec3 q2 = point - GetPosition();
  vec3 res = q1 * vec3(-1.0f + 2 * int(q2[0] > 0), -1.0f + 2 * int(q2[1] > 0),
                       -1.0f + 2 * int(q2[2] > 0));
  return res;
}

const Material& Box::GetMaterial() const {
  return *material_.get();
}

void Box::UpdateUniforms(Shader& shader) const {}

void Box::SetRenderPosition(const vec3& position) {
  SetPosition(position);
}

vec3 Box::GetRenderPosition() const {
  return GetPosition();
}

vec3 Box::GetHalfExtents() const {
  return vertex_;
}

CollisionManifold Box::ComputeCollision(const SceneObject* other,
                                        const PhysicsBody* myBody,
                                        const PhysicsBody* otherBody) const {
  return other->ComputeCollisionWith(this, otherBody, myBody);
}

// Box vs Sphere
CollisionManifold Box::ComputeCollisionWith(
    const Sphere* otherSphere,
    const PhysicsBody* myBody,
    const PhysicsBody* otherBody) const {
  CollisionManifold result;
  vec3 spherePos = otherBody->GetPosition();
  vec3 boxPos = myBody->GetPosition();

  // Получаем матрицу поворота коробки
  mat3<float> boxRot = quatToMat3(myBody->orientation);

  // Переводим сферу в локальную систему координат коробки
  vec3 relPos = spherePos - boxPos;
  vec3 localSpherePos = MulMatTransposedVec(boxRot, relPos);

  // Находим ближайшую точку на поверхности коробки к центру сферы
  vec3 boxHalfExtents = this->GetHalfExtents();
  vec3 closestPointLocal = vec3(
      std::clamp(localSpherePos[0], -boxHalfExtents[0], boxHalfExtents[0]),
      std::clamp(localSpherePos[1], -boxHalfExtents[1], boxHalfExtents[1]),
      std::clamp(localSpherePos[2], -boxHalfExtents[2], boxHalfExtents[2]));

  // Проверяем расстояние от ближайшей точки до центра сферы
  vec3 differenceLocal = localSpherePos - closestPointLocal;
  float distance = length(differenceLocal);
  float radius = otherSphere->GetRadius();

  if (distance < radius) {
    result.isColliding = true;
    result.depth = radius - distance;

    vec3 normalLocal;
    if (distance < 1e-6f) {
      // Сфера внутри - берем вектор до центра
      normalLocal = normalize(localSpherePos);
    } else {
      normalLocal = normalize(differenceLocal);
    }

    // Переводим результат обратно в мировые координаты
    // Нормаль должна указывать от Box к Sphere
    result.normal = MulMatVec(boxRot, normalLocal);
    result.contactPoint = boxPos + MulMatVec(boxRot, closestPointLocal);
  }
  return result;
}

// Box vs Box
CollisionManifold Box::ComputeCollisionWith(
    const Box* otherBox,
    const PhysicsBody* myBody,
    const PhysicsBody* otherBody) const {
  // Логика Box vs Box пока не реализована
  return {};
}
