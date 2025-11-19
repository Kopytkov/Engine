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
