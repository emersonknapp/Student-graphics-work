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
	return ((double) rand() / (RAND_MAX)) ;


}

vec3 randomSpherePoint() {
	double az,u,v,xdir,ydir,zdir,tmp;
	u = 1.0 - (2.0 * rand01());
	az = 2.0 * PI * rand01();

	tmp = 1 - u*u;
	v = sqrt(tmp);

	xdir = v * cos(az);
	ydir = v * sin(az);
	zdir = u;
	
	return vec3(xdir,ydir,zdir).normalize();

}
