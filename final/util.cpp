#include "util.h"
using namespace std;

// based on the coordinates v1,v2,v3, BARYCENTRIC returns
// the interpolated result on input
vec3 barycentric(vec3 v1, vec3 v2, vec3 v3, vec3 hitPoint) {

	vec3 barycentric;
	float t = (v1[0]-v3[0]) * (v2[1]-v3[1]) - (v1[1]-v3[1]) * (v2[0]-v3[0]);

	barycentric[0] = ( (v2[1]-v3[1]) * (hitPoint[0] - v3[0]) + (v3[0]-v2[0]) * (hitPoint[1] - v3[1])) / t;
	barycentric[1] = ( (v3[1]-v1[1]) * (hitPoint[0] - v3[0]) + (v1[0]-v3[0]) * (hitPoint[1] - v3[1])) / t;
	barycentric[2] = 1 - barycentric[0] - barycentric[1];
	return barycentric;
}

vec3 barycentric(vec4 v1, vec4 v2, vec4 v3, vec4 hitPoint) {
	return barycentric(v1.dehomogenize(), v2.dehomogenize(), v3.dehomogenize(), hitPoint.dehomogenize());
}

double rand01() {
	return ((double) rand() / (RAND_MAX));
}

vec3 randomSpherePoint() {
	double u, v, theta, phi, x, y, z;
	u = rand01();
	v = rand01();
	theta = 2 * PI * u;
	phi = acos(2*v - 1);
	x = sin(phi)*cos(theta);
	y = sin(phi)*sin(theta);
	z = cos(phi);
	return vec3(x, y, z);

}

vec3 randomHemispherePoint(vec3 normal) {
	vec3 point = randomSpherePoint();
	float cosangle = -normal * point;
	if (cosangle > 0) {
		return -point;
	} 
	return point;
}

vec3 randomHemispherePoint(vec4 normal) {
	return randomHemispherePoint(normal.dehomogenize());
}

vec3 randomCirclePoint(vec3 normal) {
	//generate random sphere points and then change the z components so that the
	//points are perpendicular to the normal
	normal.normalize();
	vec3 point = randomSpherePoint();
	point[2] = -(point[0]*normal[0] + point[1]*normal[1]) / normal[2];
	return point;
}

vec4 randomCirclePoint(vec4 normal) {
	return randomCirclePoint(normal.dehomogenize());
}

double sum(vec3 v) {
	return v[0] + v[1] + v[2];
}
