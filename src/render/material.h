#pragma once
#include "RGB.h"
#include "math/vec.h"

class Scene;
struct Hit;

class Material {
 public:
  virtual ~Material() = default;

  // Возвращает базовый цвет в точке
  virtual RGB albedo(const Hit& hit, const Scene& scene) const = 0;

  // Расчёт цвета (математика света)
  virtual vec3 shade(const Hit& hit,
                     const Scene& scene,
                     const vec3& viewDir,
                     int depth = 0) const = 0;

  // Пропускает ли материал свет
  virtual bool isTransparent() const { return false; }
};
