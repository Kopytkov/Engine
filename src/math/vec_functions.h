#pragma once

#include "vec.h"

#include <cmath>

template <typename T, int N>
vec<T, N> min(const vec<T, N> &a, const vec<T, N> &b) {
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

template <typename T, int N> vec<T, N> min(const vec<T, N> &a, const float &b) {
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
vec<T, N> max(const vec<T, N> &a, const vec<T, N> &b) {
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

template <typename T, int N> vec<T, N> max(const vec<T, N> &a, const float &b) {
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
vec<T, N> clamp(const vec<T, N> &a, const vec<T, N> &minVal,
                const vec<T, N> &maxVal) {
  return min(max(a, minVal), maxVal);
}

template <typename T, int N>
vec<T, N> clamp(const vec<T, N> &a, const float &minVal, const float &maxVal) {
  return min(max(a, minVal), maxVal);
}

template <typename T, int N>
vec<T, N> mix(const vec<T, N> &a, const vec<T, N> &b, const vec<T, N> &wb) {
  return a * (vec<T, N>(1) - wb) + b * wb;
}

template <typename T, int N>
vec<T, N> mix(const vec<T, N> &a, const vec<T, N> &b, const float &wb) {
  return a * vec<T, N>(1 - wb) + b * vec<T, N>(wb);
}

inline vec3 cross(const vec3 &a, const vec3 &b) {
  return vec3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
              a[0] * b[1] - a[1] * b[0]);
}

template <typename T, int N> float dot(const vec<T, N> &a, const vec<T, N> &b) {
  float res = 0;
  for (int i = 0; i < N; i++) {
    res += float(a[i] * b[i]);
  }
  return res;
}

template <typename T, int N> float length(const vec<T, N> &a) {
  return std::sqrt(dot(a, a));
}

template <typename T, int N>
float distance(const vec<T, N> &a, const vec<T, N> &b) {
  return length(a - b);
}

template <typename T, int N> vec<T, N> normalize(const vec<T, N> &a) {
  return a / length(a);
}

template <typename T, int N> vec<T, N> abs(const vec<T, N> &a) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = std::abs(a[i]);
  }
  return res;
}

template <typename T, int N>
vec<T, N> faceforward(const vec<T, N> &n, const vec<T, N> &i,
                      const vec<T, N> &nRef) {
  if (dot(nRef, i) < 0) {
    return n;
  } else {
    return -n;
  }
}

template <typename T, int N>
vec<T, N> reflect(const vec<T, N> &i, const vec<T, N> &n) {
  return i - T(2) * dot(n, i) * n;
}

template <typename T, int N>
vec<T, N> refract(const vec<T, N> &i, const vec<T, N> &n, float r) {
  float d = 1.0 - r * r * (1.0 - dot(n, i) * dot(n, i));
  if (d < 0.0) {
    return vec<T, N>(0.0);
  } else {
    return r * i - (r * dot(n, i) + sqrt(d)) * n;
  }
}