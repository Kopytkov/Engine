#pragma once

#include "gl/shader.h"

class AppUtils {
 public:
  // Настройка рендеринга шаров через raymarching
  static bool SetupRaymarchBallRendering(Shader& raymarchShader);
};
