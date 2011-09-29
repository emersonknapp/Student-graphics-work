#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>
#ifdef _WIN32
#	include <windows.h>
#else
#	include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#include "classes.h"




using namespace std;

Material::Material() {
	ka = vec3(0,0,0);
	kd = vec3(0,0,0);
	ks = vec3(0,0,0);
	kr = 0;
	sp = 0;
}

Material::Material(vec3 a, vec3 d, vec3 s, float r, int p) {
	ka = a;
	kd = d;
	ks = s;
	sp = p;
	kr = r;
}

	
Ray::Ray(vec4 a, vec4 b) {
	pos = a;
	dir = b;
}
	


Renderable::Renderable () {
	tmat = mat4(
			vec4(0,0,0,0),
			vec4(0,0,0,0),
			vec4(0,0,0,0),
			vec4(1,0,0,0)
			);
}
Renderable::Renderable (int x, int y, int z) {
	tmat = mat4(
			vec4(x,0,0,0),
			vec4(y,0,0,0),
			vec4(z,0,0,0),
			vec4(1,0,0,0)
			);	
}

void Renderable::translate (int x, int y, int z) { // generates translation matrix and updates tmat
	mat4 m = mat4(
				vec4(1,0,0,x),
		 		vec4(0,1,0,y),
		 		vec4(0,0,1,z),
		 		vec4(0,0,0,1)
				);
	tmat = tmat * m;
	imat = tmat.inverse();
}
void Renderable::rotate(int angle, vec3 u) { // generates rotation matrix and updates tmat. rotates angle around vector u
	u.normalize();
	float factor = sin(angle/2.0f);
	vec4 q = vec4(cos(angle/2.0f),u[0]*factor,u[1]*factor,u[2]*factor);
	int w = q[0];
	int x = q[1];
	int y = q[2];
	int z = q[3];
	int ww = pow(q[0],2);
	int xx = pow(q[1],2);
	int yy = pow(q[2],2);
	int zz = pow(q[3],2);
	mat4 r = mat4(
				vec4(ww+xx-yy-zz,2*x*y+2*w*z,2*x*z-2*w*y,0),
				vec4(2*x*y-2*w*z,ww-xx+yy-zz,2*y*z+2*w*x,0),
				vec4(2*x*z+2*w*y,2*y*z-2*w*z,ww-xx-yy+zz,0),
				vec4(0,0,0,ww+xx+yy+zz)
			);
	tmat = tmat * r;
	imat = tmat.inverse();
}
void Renderable::scale(int xScale, int yScale, int zScale) { // generates scale matrix and updates tmat
	mat4 s = mat4(
				vec4(xScale,0,0,0),
				vec4(0,yScale,0,0),
				vec4(0,0,zScale,0),
				vec4(0,0,0,1)
				);
	tmat = tmat * s;
	imat = tmat.inverse();
}


Camera::Camera() {
	pos = vec3(0,0,1);
	up = vec3(0,1,0);
	viewer = vec3(0,0,-1);
}

bool Camera::ray_intersect (Ray &r, int &t, vec4 &normal) {
	return false;
}


Sphere::Sphere(int a) {
	radius = a;
	vec4 base = vec4(0,0,0,1);
}

bool Sphere::ray_intersect ( Ray &r, int &t, vec4 &normal) {
	vec4 pos = tmat * base;
	float a = r.dir.length2();
	float b = 2*r.pos*r.dir + pos * r.dir;
	float c = r.pos.length2() - r.pos*pos + pos.length2() - pos*r.pos - pow(radius,2.0f);
	if (pow(b,2)+4*a*c < 0 ) {
		return false;
	} else {
		// this t determines intersection point A + t*D = r.pos + t * r.dir
		t = min((-b + sqrt(pow(b,2)-4*a*c) / (2*a) ), (-b + sqrt(pow(b,2)-4*a*c)) / (2*a));
		vec4 intersection = r.pos + t * r.dir; // this is a point on the sphere
		// if we have normal n on the sphere, then if it's transformed, we use (M^-1)^T * n as the normal
		// intersection - pos is the normal on the sphere
		normal = tmat.inverse().transpose() * (intersection - pos);
		normal.normalize();

		return true;
	}
}

Triangle::Triangle (vec4 a, vec4 b, vec4 c) {
	v1 = a;
	v2 = b;
	v3 = c;
}
	
bool Triangle::ray_intersect ( Ray &r, int &t, vec4 &normal ) {
	// res : Beta | gamma | t
	vec3 res = mat4(
					vec4((v2-v1)[0],(v3-v1)[0],-r.dir[0],0),
					vec4((v2-v1)[1],(v3-v1)[1],-r.dir[1],0),
					vec4((v2-v1)[2],(v3-v1)[3],-r.dir[2],0),
					vec4(0,0,0,0)
					).inverse() * vec4(r.pos) - v1;
	if (res[0] > 0 && res[1] > 0 && res[0]+res[1] <= 1 && res[2] < t) {
		t = res[2];
		vec4 intersection = r.pos + t * r.dir; // this is a point on the triangle
		normal = tmat.inverse().transpose() * ((v1-t) ^ (v3-t)); // the second part is the cross product of two vectors that define the triangle from point t
		normal.normalize();
		return true;
	} else {
		return false;
	}
}
