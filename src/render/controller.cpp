#include "controller.h"

PositionController::PositionController(PositionProperty* prop): prop_(prop){};

void PositionController::Update(float dt, vec3 velocity) const{
    prop_->SetPosition(prop_->GetPosition() + dt*velocity);
}