#ifndef _util_H
#define _util_H

#include "algebra3.h"
#include <cmath>
#include <limits>

vec3 barycentric(vec3, vec3, vec3, vec3);

vec3 barycentric(vec4, vec4, vec4, vec4);

double rand01();

#endif
