#include "ray.h"
#include "math\vec_functions.h"

Ray::Ray(const vec3& pos, const vec3& dir, int n)
    : position(pos), numOfStep(n) {
  direction = normalize(dir);
}
