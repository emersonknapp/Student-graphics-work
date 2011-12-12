#include "classes.h"

vec4 Light::normal(vec4 hitPoint) { return vec4(0,0,0,0); }
vec3 Light::textureColor(vec4 hitPoint) { return vec3(0,0,0); }
vec4 Light::randomSurfacePoint() { return pos; }
float Light::minorArea() { return 0; }
float Light::rayIntersect(Ray r) { return -1; }


PLight::PLight(vec4 p, vec3 i) {
	pos = p;
	intensity = i;
}

PLight::PLight(vec4 p, vec3 i, float _pow) {
	pos = p;
	intensity = i;
	power = _pow;
}

vec4 PLight::lightVector(vec4 origin) {
	return pos-origin;
}

void PLight::generatePhotons(vector<Photon*>& photonCloud, int numPhots, AABB* s) {
	vec3 photensity = (power*intensity)/numPhots;
	for (int i=0; i<numPhots; i++) {
		vec3 photonDir = randomSpherePoint(); 
		Photon* photon = new Photon(pos, vec4(photonDir,0), photensity);
		photonCloud.push_back(photon);
	}
	
}

DLight::DLight(vec4 d, vec3 i) {
	pos = d;
	intensity = i;
}

vec4 DLight::lightVector(vec4 origin) {
	return -pos;
}

void DLight::generatePhotons(vector<Photon*>& photonCloud, int numPhots, AABB* s) {
	// centered at lower right corner of plane
	vec4 center;
	float scale;
	for (int i = 0; i < 3; i++) {
		if (pos[i]>0) center[i] = s->mins[i];
		else if (pos[i]<0) center[i] = s->maxes[i];
		else if (pos[i]==0) center[i] = 0;
	}
	center[3] = 1;
	

	float max = (s->mins - s->maxes).length() / 2; 
	for (int i=0; i<numPhots; i++) {
		scale = max * rand01();
		vec4 photonPos = center + scale * vec4(randomCirclePoint(pos),0);
		Photon* photon = new Photon(photonPos, pos, intensity);
		photonCloud.push_back(photon);
	}
}

vec4 TriLight::normal(vec4 hitPoint) { 
	vec4 n = tmat * vec4((v1-v3).dehomogenize()^(v2-v3).dehomogenize(),0);	
	return n;
}

vec4 TriLight::randomSurfacePoint() {
	float a = rand01();
	float b = rand01();
	if (a+b > 1) {
		a = 1.0f-a;
		b = 1.0f-b;
	}
	float c = 1-a-b;
	return v1*a + v2*b + v3*c;
}

float TriLight::minorArea() { return 0.5 * ( (v3-v1) ^ (v2-v1) ).length(); }

float TriLight::rayIntersect(Ray r) {
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	raydir.normalize();
	float t;
	vec4 u = v2-v3;
	vec4 v = v1-v3;
	vec4 norm = vec4((u.dehomogenize()^v.dehomogenize()).normalize(),0);
	if (fabs(norm * raydir) == 0) {return -1;}		
	if (norm == vec4(0,0,0,0)) {return -1;}
	vec4 w0 = raypos - v3;
	float a = -(norm * w0);
	float b = (norm * raydir);
	if (fabs(b) < WIGGLE) { //ray is parallel to triangle
		return -1;
	}
	// get ray intersect point
	t = a / b; // t is the parametric intersection of ray with triangle
	if (t < 0.0) return -1; //ray pointing away from triangle
	// check if the ray intersects within the triangle (isntead of just the triangle plane)
	float uu, uv, vv, wu, wv, D;
	uu = u*u;
	uv = u*v;
	vv = v*v;
	vec4 w = (raypos + t*raydir - v3);
	wu = w*u;
	wv = w*v;
	D = uv * uv - uu * vv;
	float s,f;
	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 or s > 1.0) return -1; // outside
	f = (uv * wu - uu * wv) / D;
	if (f < 0.0 or (s+f) > 1.0) return -1; // outside
	return t;
}

