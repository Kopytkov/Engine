#pragma once

#include "math/vec.h"
#include "raw_image.h"


class LightSource{
    
    float brightness;
    RGB color;

    public:

    LightSource(float _brightness, RGB _color);
    
    float getBrightness() const;
    RGB getColor() const;

    virtual vec3 lightDirection(vec3 point) const = 0;

};

using LightSources = std::vector<std::unique_ptr<LightSource>>;



