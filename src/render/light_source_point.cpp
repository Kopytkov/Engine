#include "light_source_point.h"
#include "math/vec_functions.h"

PointLightSource::PointLightSource(vec3 _position,
                                   float _brightness,
                                   RGB _color)
    : LightSource::LightSource(_brightness, _color) {
  position = _position;
}

vec3 PointLightSource::lightDirection(vec3 point) const {
  return normalize(position - point);
}
