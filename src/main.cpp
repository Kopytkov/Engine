#include <iostream>
#include "core/application.h"

int main(int argc, char* argv[]) {
  // Создаем экземпляр приложения
  Application app;

  // Инициализируем ресурсы
  if (!app.Initialize()) {
    std::cerr << "Application initialization failed. See logs for details."
              << std::endl;
    return -1;
  }

  // Запускаем игровой цикл
  app.Run();

  return 0;
}
