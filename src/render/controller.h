#pragma once
#include "property.h"

class PositionController{

    PositionProperty* prop_;

    public:
    PositionController(PositionProperty* prop);
    void Update(float dt, vec3 velocity) const;
};