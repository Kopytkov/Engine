#include "camera_controller.h"
#include "math/vec_functions.h"

CameraController::CameraController(Camera& cam, const InputController& inp)
    : camera(cam), input(inp), moveSpeed(3.0f), rotateSpeed(90.0f) {}

void CameraController::SetSpeed(float mv, float rot) {
  moveSpeed = mv;
  rotateSpeed = rot;
}

void CameraController::HandleInput(float deltaTime) {
  // Вращение
  int mx = input.GetMouseDeltaX();
  int my = input.GetMouseDeltaY();

  if (mx != 0 || my != 0) {
    camera.Rotate(static_cast<float>(mx) * rotateSpeed * deltaTime,
                  -static_cast<float>(my) * rotateSpeed * deltaTime);
  }

  // Перемещение
  if (input.IsKeyDown(SDL_SCANCODE_W)) {
    camera.Move(camera.GetUpVec() * moveSpeed * deltaTime);
  }
  if (input.IsKeyDown(SDL_SCANCODE_S)) {
    camera.Move(-camera.GetUpVec() * moveSpeed * deltaTime);
  }
  if (input.IsKeyDown(SDL_SCANCODE_A)) {
    vec3 left = normalize(cross(camera.GetUpVec(), camera.GetViewVec()));
    camera.Move(left * moveSpeed * deltaTime);
  }
  if (input.IsKeyDown(SDL_SCANCODE_D)) {
    vec3 right = normalize(cross(camera.GetViewVec(), camera.GetUpVec()));
    camera.Move(right * moveSpeed * deltaTime);
  }
  if (input.IsKeyDown(SDL_SCANCODE_SPACE)) {
    camera.Move(camera.GetViewVec() * moveSpeed * deltaTime);
  }
  if (input.IsKeyDown(SDL_SCANCODE_LCTRL)) {
    camera.Move(-camera.GetViewVec() * moveSpeed * deltaTime);
  }
}
