#include "property.h"

void PositionProperty::SetPosition(vec3 position){
    position_ = position;
}

vec3 PositionProperty::GetPosition() const{
    return position_;
}