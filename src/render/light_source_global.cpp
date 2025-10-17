#include "light_source_global.h"
#include "math/vec_functions.h"

GlobalLight::GlobalLight(vec3 _direction, float _brightness, RGB _color) : LightSource::LightSource(_brightness, _color) {
    direction = _direction;
}

vec3 GlobalLight::getDirection() const{
    return direction;
}

vec3 GlobalLight::lightDirection(vec3 point) const{
    return - normalize(direction);
}