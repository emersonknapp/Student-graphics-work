#include "classes.h"

vec4 Light::normal(vec4) { return vec4(0,0,0,0); }
vec3 Light::textureColor(vec4) { return vec3(0,0,0); }
vec4 Light::randomSurfacePoint() { return pos; }
float Light::minorArea() { return 0; }
float Light::rayIntersect(Ray) { return -1; }


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