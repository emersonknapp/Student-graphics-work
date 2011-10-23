#include "classes.h"

using namespace std;

mat4 Identity4 = mat4(vec4(1, 0, 0, 0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1));


Material::Material() {
	kd = vec3(1,1,1);
	ks = vec3(0,0,0);
	kr = vec3(0,0,0);
	sp = 0;
}

Material::Material(vec3 d, vec3 s, vec3 r, int p) {
	kd = d;
	ks = s;
	sp = p;
	kr = r;
}

Camera::Camera() {
	pos = vec4(0,0,0,1);
	UL = vec4(-1, 1, -3, 1);
	UR = vec4(1,1,-3,1);
	LL = vec4(-1,-1, -3,1);
	LR = vec4(1,-1,-3,1);

}


Viewport::Viewport () {
	w = 0;
	h = 0;
}
Viewport::Viewport (int width, int height) {
	w = width;
	h = height;
}

PLight::PLight(vec4 p, vec3 i) {
	v = p;
	intensity = i;
}

DLight::DLight(vec4 d, vec3 i) {
	v = d;
	intensity = i;
}

vec4 PLight::lightVector(vec4 origin) {
	return v-origin;
}

vec4 DLight::lightVector(vec4 origin) {
	return -v;
}

