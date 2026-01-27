#pragma once

#include <algorithm>
#include <cmath>
#include "render/RGB.h"
#include "vec.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

template <typename T, int N>
vec<T, N> min(const vec<T, N>& a, const vec<T, N>& b) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    if (a[i] < b[i]) {
      res[i] = a[i];
    } else {
      res[i] = b[i];
    }
  }
  return res;
}

template <typename T, int N>
vec<T, N> min(const vec<T, N>& a, const float& b) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    if (a[i] < b) {
      res[i] = a[i];
    } else {
      res[i] = b;
    }
  }
  return res;
}

template <typename T, int N>
vec<T, N> max(const vec<T, N>& a, const vec<T, N>& b) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    if (a[i] > b[i]) {
      res[i] = a[i];
    } else {
      res[i] = b[i];
    }
  }
  return res;
}

template <typename T, int N>
vec<T, N> max(const vec<T, N>& a, const float& b) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    if (a[i] > b) {
      res[i] = a[i];
    } else {
      res[i] = b;
    }
  }
  return res;
}

template <typename T, int N>
vec<T, N> clamp(const vec<T, N>& a,
                const vec<T, N>& minVal,
                const vec<T, N>& maxVal) {
  return min(max(a, minVal), maxVal);
}

template <typename T, int N>
vec<T, N> clamp(const vec<T, N>& a, const float& minVal, const float& maxVal) {
  return min(max(a, minVal), maxVal);
}

template <typename T, int N>
vec<T, N> mix(const vec<T, N>& a, const vec<T, N>& b, const vec<T, N>& wb) {
  return a * (vec<T, N>(1) - wb) + b * wb;
}

template <typename T, int N>
vec<T, N> mix(const vec<T, N>& a, const vec<T, N>& b, const float& wb) {
  return a * vec<T, N>(1 - wb) + b * vec<T, N>(wb);
}

inline vec3 cross(const vec3& a, const vec3& b) {
  return vec3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
              a[0] * b[1] - a[1] * b[0]);
}

template <typename T, int N>
T dot(const vec<T, N>& a, const vec<T, N>& b) {
  T res = T(0);
  for (int i = 0; i < N; i++) {
    res += a[i] * b[i];
  }
  return res;
}

template <typename T, int N>
float length(const vec<T, N>& a) {
  return std::sqrt(dot(a, a));
}

template <typename T, int N>
float distance(const vec<T, N>& a, const vec<T, N>& b) {
  return length(a - b);
}

template <typename T, int N>
vec<T, N> normalize(const vec<T, N>& a) {
  return a / length(a);
}

template <typename T, int N>
vec<T, N> abs(const vec<T, N>& a) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = std::abs(a[i]);
  }
  return res;
}

template <typename T, int N>
vec<T, N> faceforward(const vec<T, N>& n,
                      const vec<T, N>& i,
                      const vec<T, N>& nRef) {
  if (dot(nRef, i) < 0) {
    return n;
  } else {
    return -n;
  }
}

template <typename T, int N>
vec<T, N> reflect(const vec<T, N>& i, const vec<T, N>& n) {
  return i - T(2) * dot(n, i) * n;
}

template <typename T, int N>
vec<T, N> refract(const vec<T, N>& i, const vec<T, N>& n, float r) {
  const auto dot_p = dot(n, i);
  float d = 1.0f - r * r * (1.0f - dot_p * dot_p);

  if (d < 0.0f) {
    return vec<T, N>(0.0f);
  } else {
    return r * i - (r * dot_p + std::sqrt(d)) * n;
  }
}

template <typename T, int N>
vec<T, N> pow(const vec<T, N>& v, T exponent) {
  vec<T, N> res;
  for (int i = 0; i < N; ++i) {
    res[i] = std::pow(v[i], exponent);
  }
  return res;
}

template <typename T, int N>
vec<T, N> pow(const vec<T, N>& v1, const vec<T, N>& v2) {
  vec<T, N> res;
  for (int i = 0; i < N; ++i) {
    res[i] = std::pow(v1[i], v2[i]);
  }
  return res;
}

inline float saturate(float x) {
  return std::max(0.0f, std::min(1.0f, x));
}

// Умножение Матрицы на Вектор
inline vec3 MulMatVec(const mat3<float>& m, const vec3& v) {
  return vec3(dot(m[0], v),  // Строка 0 * v
              dot(m[1], v),  // Строка 1 * v
              dot(m[2], v)   // Строка 2 * v
  );
}

// Умножение Транспонированной Матрицы на Вектор
inline vec3 MulMatTransposedVec(const mat3<float>& m, const vec3& v) {
  return vec3(
      m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2],  // Столбец 0 * v
      m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2],  // Столбец 1 * v
      m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2]   // Столбец 2 * v
  );
}

inline vec3 RGBtoVec3(const RGB& c) {
  return vec3(static_cast<float>(c.r) / 255.0f,
              static_cast<float>(c.g) / 255.0f,
              static_cast<float>(c.b) / 255.0f);
}

inline RGB Vec3toRGB(const vec3& c) {
  auto clamp255 = [](float v) -> uint8_t {
    return static_cast<uint8_t>(std::clamp(v * 255.0f, 0.0f, 255.0f));
  };
  return RGB{clamp255(c[0]), clamp255(c[1]), clamp255(c[2])};
}

// Постобработка: Гамма-коррекция (sRGB)
inline vec3 applyGamma(vec3 color, float gamma = 2.2f) {
  return pow(color, 1.0f / gamma);
}

// Тонмаппинг: Reinhard
inline vec3 reinhardTonemap(vec3 color) {
  return color / (color + vec3(1.0f));
}

// Тонмаппинг: ACES
inline vec3 acesTonemap(vec3 color) {
  const float a = 2.51f;
  const float b = 0.03f;
  const float c = 2.43f;
  const float d = 0.59f;
  const float e = 0.14f;
  return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f,
               1.0f);
}

// Нормализация кватерниона
inline quat normalize(const quat& q) {
  float len = std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
  if (len < 1e-6f) {
    return quat(1, 0, 0, 0);
  }
  float inv = 1.0f / len;
  return quat(q.w * inv, q.x * inv, q.y * inv, q.z * inv);
}

// Умножение кватернионов
inline quat operator*(const quat& q1, const quat& q2) {
  return quat(q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
              q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
              q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
              q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w);
}

// Умножение кватерниона на скаляр
inline quat operator*(const quat& q, float s) {
  return quat(q.w * s, q.x * s, q.y * s, q.z * s);
}

// Сложение кватернионов
inline quat operator+(const quat& a, const quat& b) {
  return quat(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
}

// Конвертация Кватерниона в Матрицу 3x3
inline mat3<float> quatToMat3(const quat& q) {
  mat3<float> m;
  float x2 = q.x + q.x;
  float y2 = q.y + q.y;
  float z2 = q.z + q.z;
  float xx = q.x * x2;
  float xy = q.x * y2;
  float xz = q.x * z2;
  float yy = q.y * y2;
  float yz = q.y * z2;
  float zz = q.z * z2;
  float wx = q.w * x2;
  float wy = q.w * y2;
  float wz = q.w * z2;

  m[0] = vec3(1.0f - (yy + zz), xy - wz, xz + wy);
  m[1] = vec3(xy + wz, 1.0f - (xx + zz), yz - wx);
  m[2] = vec3(xz - wy, yz + wx, 1.0f - (xx + yy));
  return m;
}
