#include "input_controller.h"
#include <cstring>

InputController::InputController()
    : keyboardState(nullptr),
      mouseDeltaX(0),
      mouseDeltaY(0),
      shouldClose(false) {
  std::memset(prevKeyboardState, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);
}

InputController::~InputController() {
  SDL_SetRelativeMouseMode(SDL_FALSE);
}

void InputController::Initialize() {
  // Получаем прямой доступ к массиву состояния клавиатуры
  keyboardState = SDL_GetKeyboardState(nullptr);

  // Скрываем курсор и захватываем его в окне
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void InputController::PollEvents() {
  // Сохраняем прошлое состояние
  if (keyboardState) {
    std::memcpy(prevKeyboardState, keyboardState,
                sizeof(Uint8) * SDL_NUM_SCANCODES);
  }

  // Сбрасываем дельты мыши
  mouseDeltaX = 0;
  mouseDeltaY = 0;

  // Обрабатываем все события из очереди SDL
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ProcessEvent(event);
  }

  // Обновляем относительное смещение мыши
  int x, y;
  SDL_GetRelativeMouseState(&x, &y);
  mouseDeltaX += x;
  mouseDeltaY += y;

  // Обновляем указатель на массив клавиш
  keyboardState = SDL_GetKeyboardState(nullptr);
}

bool InputController::IsKeyDown(SDL_Scancode key) const {
  if (!keyboardState) {
    return false;
  }
  return keyboardState[key] != 0;
}

bool InputController::IsKeyPressed(SDL_Scancode key) const {
  if (!keyboardState) {
    return false;
  }
  return keyboardState[key] && !prevKeyboardState[key];
}

int InputController::GetMouseDeltaX() const {
  return mouseDeltaX;
}

int InputController::GetMouseDeltaY() const {
  return mouseDeltaY;
}

bool InputController::ShouldClose() const {
  return shouldClose;
}

void InputController::ProcessEvent(const SDL_Event& event) {
  switch (event.type) {
    // Закрытие окна
    case SDL_QUIT:
      shouldClose = true;
      break;
    // Нажатие клавиш
    case SDL_KEYDOWN:
      if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        shouldClose = true;
      }
      break;
    default:
      break;
  }
}
