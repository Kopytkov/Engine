#include "property.h"

void PositionProperty::SetPosition(const vec3& position) {
  position_ = position;
}

const vec3& PositionProperty::GetPosition() const {
  return position_;
}
