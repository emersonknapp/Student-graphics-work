#include "classes.h"

using namespace std;

Material::Material() {
	ka = vec3(.1,.1,.1);
	kd = vec3(1,1,1);
	ks = vec3(0,0,0);
	kr = vec3(0,0,0);
	sp = 0;
	ri = 0.0;
}

Material::Material(vec3 a, vec3 d, vec3 s, vec3 r, int p) {
	ka = a;
	kd = d;
	ks = s;
	sp = p;
	kr = r;
}

Ray::Ray() {
	pos = vec4(0,0,0,0);
	dir = vec4(0,0,0,0);
	refracted = false;
	curRI = 1.0;
	oldRI = 1.0;
}	

Ray::Ray(vec4 a, vec4 b) {
	pos = a;
	dir = b;
	dir.normalize();
	refracted = false;
	curRI = 1.0;
	oldRI = 1.0;
}	

Ray::Ray(vec4 a, vec4 b, float r) {
	pos = a;
	dir = b;
	dir.normalize();
	curRI = r;
	oldRI = 1.0;
}

Ray::Ray(vec4 a, vec4 b, bool t) {
	pos = a;
	dir = b;
	dir.normalize();
	refracted = t;
	curRI = 1.0;
	oldRI = 1.0;
}


Ray::Ray(vec4 a, vec4 b, float rn, float ro, bool t) {
	pos = a;
	dir = b;
	dir.normalize();
	curRI = rn;
	oldRI = ro;
	refracted = t;
}

Texture::Texture() {}

Texture::Texture(const char* fn) {
	txt = FreeImage_Load(FIF_PNG, fn, PNG_DEFAULT);
	width = FreeImage_GetWidth(txt);
	height = FreeImage_GetHeight(txt);
}

vec3 Texture::getColor(float u, float v) {
//TODO: currently, this just gets pixels from a png input, but I want to enable function that generate textures too
	RGBQUAD color;
	float x = u * width;
	float y = v * height;

	FreeImage_GetPixelColor(txt, x, y, &color);
	float r = color.rgbRed / 255.0f;
	float g = color.rgbGreen / 255.0f;
	float b = color.rgbBlue / 255.0f;
	return vec3(r,g,b);
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

AABB::AABB() {
	mins = vec3(INT_MAX, INT_MAX, INT_MAX);
	maxes = vec3(INT_MIN, INT_MIN, INT_MIN);
}

bool AABB::rayIntersect(Ray r) {
	vec3 tmin; //tvalues at which boxmins are hit
	vec3 tmax; //tvalues at which boxmaxes are hit
	
	
	
	return false; //TODO: this
}

//Makes this aabb the concatenation of itself and OTHER
void AABB::concat(AABB* other) {
	for (int i=0; i<3; i++) {
		mins[i] = min(mins[i], other->mins[i]);
		maxes[i] = max(maxes[i], other->maxes[i]);
	}
}

void AABB::print(int indent) {
	cout << string(indent*2, ' ') << "Bounding volume: " << endl;
	cout << string(indent*2, ' ') << mins << maxes << endl;
}
