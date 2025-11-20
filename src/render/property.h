#pragma once

#include "math/vec.h"

class PositionProperty{

    private:
    
    vec3 position_;

    public:

    vec3 GetPosition() const;

    void SetPosition(vec3 position);
};