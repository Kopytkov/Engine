#include "controller.h"

PositionController::PositionController(PositionProperty* prop) : prop_(prop){};

void PositionController::Update(float dt) const {
  prop_->SetPosition(prop_->GetPosition() + dt * vec3(0, 1, 0));
}
