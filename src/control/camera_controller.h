#pragma once

#include "input/input_controller.h"
#include "render/camera.h"

class CameraController {
 public:
  CameraController(Camera& camera, const InputController& input);

  void SetSpeed(float moveSpeed, float rotateSpeed);

  // Читает ввод и двигает камеру
  void HandleInput(float deltaTime);

 private:
  Camera& camera;
  const InputController& input;

  float moveSpeed;
  float rotateSpeed;
};
