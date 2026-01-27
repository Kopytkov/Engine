#pragma once

#include <iostream>

inline constexpr float kMinDistance = 0.001f;
inline constexpr uint32_t kMaxStep = 300;
inline constexpr float kMaxDistance = 1500.0f;

template <typename T, int N>
class vec {
 private:
  T data[N] = {T(0)};

 public:
  vec() = default;
  ~vec() = default;

  explicit vec(T a) {
    for (auto& e : data) {
      e = a;
    }
  }

  vec(T a, T b) {
    data[0] = a;
    data[1] = b;
    data[2] = 0;
  }

  vec(T a, T b, T c) {
    data[0] = a;
    data[1] = b;
    data[2] = c;
  }

  vec(const vec<T, N>& v1) {
    for (int i = 0; i < N; i++) {
      data[i] = v1[i];
    }
  }

  vec(const std::array<T, N>& arr) {
    for (int i = 0; i < N; ++i) {
      data[i] = arr[i];
    }
  }

  template <typename... Args>
  vec(Args... args) noexcept : data{static_cast<T>(args)...} {}

  vec<T, N> operator+(const vec<T, N>& v1) {
    vec<T, N> res;
    for (int i = 0; i < N; i++) {
      res[i] = this->data[i] + v1[i];
    }
    return res;
  }

  vec<T, N> operator-() const {
    vec<T, N> res;
    for (int i = 0; i < N; i++) {
      res[i] = -data[i];
    }
    return res;
  }

  const T& operator[](int i) const { return data[i]; }

  T& operator[](int i) { return data[i]; }
};

// 3x3 Матрица
template <typename T>
struct mat3 {
  vec<T, 3> rows[3];

  mat3() {
    rows[0] = vec<T, 3>(1, 0, 0);
    rows[1] = vec<T, 3>(0, 1, 0);
    rows[2] = vec<T, 3>(0, 0, 1);
  }

  const vec<T, 3>& operator[](int i) const { return rows[i]; }
  vec<T, 3>& operator[](int i) { return rows[i]; }
};

// Кватернион
struct quat {
  float w, x, y, z;
  quat() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
  quat(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}
};

template <typename T, int N>
vec<T, N> operator+(const vec<T, N>& v1, const vec<T, N>& v2) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v1[i] + v2[i];
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator-(const vec<T, N>& v1, const vec<T, N>& v2) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v1[i] - v2[i];
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator*(const vec<T, N>& v1, const vec<T, N>& v2) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v1[i] * v2[i];
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator*(const vec<T, N>& v, T a) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v[i] * a;
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator*(T a, const vec<T, N>& v) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v[i] * a;
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator/(const vec<T, N>& v, const T& a) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v[i] / a;
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator/(const vec<T, N>& v1, const vec<T, N>& v2) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = v1[i] / v2[i];
  }
  return res;
}

template <typename T, int N>
vec<T, N> operator/(const T& a, const vec<T, N>& v) {
  vec<T, N> res;
  for (int i = 0; i < N; i++) {
    res[i] = a / v[i];
  }
  return res;
}

template <typename T, int N>
vec<T, N>& operator+=(vec<T, N>& v1, const vec<T, N>& v2) {
  for (int i = 0; i < N; i++) {
    v1[i] += v2[i];
  }
  return v1;
}

template <typename T, int N>
vec<T, N>& operator-=(vec<T, N>& v1, const vec<T, N>& v2) {
  for (int i = 0; i < N; i++) {
    v1[i] -= v2[i];
  }
  return v1;
}

template <typename T, int N>
vec<T, N>& operator*=(vec<T, N>& v, T a) {
  for (int i = 0; i < N; i++) {
    v[i] *= a;
  }
  return v;
}

template <typename T, int N>
std::ostream& operator<<(std::ostream& os, const vec<T, N>& v) {
  std::ostream& res = os << "(" << v[0];
  for (int i = 1; i < N; i++) {
    res << ", " << v[i];
  }
  res << ")";
  return res;
}

using vec3 = vec<float, 3>;
using vec2 = vec<float, 2>;
