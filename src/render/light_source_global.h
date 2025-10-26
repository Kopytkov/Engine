#include "light_source.h"

class GlobalLight : public LightSource {
  vec3 direction;

 public:
  GlobalLight(vec3 _direction, float _brightness, RGB _color);

  vec3 getDirection() const;
  vec3 lightDirection(vec3 point) const override;
};
