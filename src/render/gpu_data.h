#pragma once
#include "math/vec.h"

// Структура PBR материала (32 байта)
struct GPUMaterial {
  // Блок 1 (16 байт)
  float r, g, b;    // Базовый цвет
  float roughness;  // Шероховатость

  // Блок 2 (16 байт)
  float metallic;      // Металличность
  float transmission;  // Прозрачность
  float refraction;    // Коэффициент преломления
  int textureID;       // Индекс текстурного юнита
};

// Структура одного шара (96 байт)
struct GPUBall {
  // Позиция (16 байт)
  // vec3 position занимает 12 байт, но в std140 выравнивание идет по 16 байт
  float px, py, pz;
  float _pad0;  // Выравнивание до 16 байт

  // Вращение (48 байт)
  // В std140 матрица mat3 хранится как массив из 3-х векторов vec4
  float rotCol0[4];
  float rotCol1[4];
  float rotCol2[4];

  // Материал (32 байта)
  GPUMaterial material;
};

// Главный блок данных, передаваемый в Uniform Buffer Object (UBO)
struct UBOData {
  // Глобальные параметры (16 байт)
  float ballRadius;  // Радиус всех шаров
  int ballCount;     // Текущее количество шаров
  float _pad1;       // Выравнивание до 16 байт
  float _pad2;       // Выравнивание до 16 байт

  // Массив шаров
  GPUBall balls[16];
};
