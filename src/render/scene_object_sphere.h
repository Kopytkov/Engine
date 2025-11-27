#include "scene_object.h"
#include "control/property.h"

class Sphere : public SceneObject, public PositionProperty {
 public:
  Sphere(const vec3& position, float r, std::unique_ptr<Material> mat);
  float SDF(const vec3& point) const override;
  vec3 getNormal(const vec3& point) const override;
  const Material& GetMaterial() const override;

 private:
  float radius_;
  std::unique_ptr<Material> material_;
};
