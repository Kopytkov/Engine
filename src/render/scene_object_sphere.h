#include "scene_object.h"

class Sphere : public SceneObject {
 public:
  Sphere(const vec3& position, float r, const Material& mat);
  float SDF(const vec3& point) const override;
  vec3 getNormal(const vec3& point) const override;
  const Material& GetMaterial() const override;

 private:
  vec3 position_;
  float radius_;
  Material material_;
};
