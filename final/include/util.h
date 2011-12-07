#ifndef _util_H
#define _util_H

#include "algebra3.h"
#include <cmath>
#include <limits>

#define VERSION_NUMBER "0.1"
#define PI 3.14159265
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define FRAMERATE 10
#define EPSILON 0.001
#define DEBUG false
#define T_MAX 400
#define GATHER_RAYS 10

#define MAXRECURSION 4
#define MAXLINE 255

vec3 barycentric(vec3, vec3, vec3, vec3);

vec3 barycentric(vec4, vec4, vec4, vec4);

double rand01();

vec3 randomSpherePoint();
vec3 randomHemispherePoint(vec3);
vec3 randomHemispherePoint(vec4);
vec3 randomCirclePoint(vec3);
vec4 randomCirclePoint(vec4);

double sum(vec3);

#endif
