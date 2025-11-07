#include "light_source.h"

class PointLightSource : public LightSource {
  vec3 position;

 public:
  PointLightSource(vec3 pos, float br, RGB col);

  vec3 getPosition();
  vec3 lightDirection(vec3 point) const override;
  vec3 getPosition() const override;
};
