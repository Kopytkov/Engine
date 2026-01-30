#pragma once

#include <SDL2/SDL.h>

class InputController {
 public:
  InputController();
  ~InputController();

  void Initialize();

  // Опрашивает очередь событий SDL и обновляет состояние устройств
  void PollEvents();

  // Возвращает true, пока клавиша удерживается
  bool IsKeyDown(SDL_Scancode key) const;

  // Возвращает true только в момент нажатия клавиши
  bool IsKeyPressed(SDL_Scancode key) const;

  int GetMouseDeltaX() const;
  int GetMouseDeltaY() const;
  bool ShouldClose() const;

 private:
  const Uint8* keyboardState;
  Uint8 prevKeyboardState[SDL_NUM_SCANCODES];

  int mouseDeltaX;
  int mouseDeltaY;
  bool shouldClose;

  // Внутренний метод для обработки системного события
  void ProcessEvent(const SDL_Event& event);
};
