#include "light_source.h"

LightSource::LightSource(float _brightness, RGB _color){
    brightness = _brightness;
    color = _color;
}

float LightSource::getBrightness() const{
    return brightness;
}

RGB LightSource::getColor() const{
    return color;
}
