#include "ray.h"

Ray::Ray(const vec3 &pos, const vec3 &dir, int n)
    : position(pos), direction(dir), numOfStep(n) {}