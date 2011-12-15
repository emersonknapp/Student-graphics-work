#include "classes.h"

vec4 Light::shadowCheck(vec4 point) {
	return lightVector(point);
}

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

AreaLight::AreaLight(vec4 i, vec4 j, vec4 k, vec3 c, float pow) : Triangle(i, j, k) {
	intensity = c;
	power = pow;
	pos = center;
}

void AreaLight::generatePhotons(vector<Photon*>& photonCloud, int numPhots, AABB* s) {
	
}

vec4 AreaLight::lightVector(vec4 point) {
	pos = center;
	return (center-point).normalize();
}

vec4 AreaLight::shadowCheck(vec4 point) {
	vec4 p = (center-point).normalize();
	//cout << p << endl;
	pos = randomSurfacePoint();
	return (pos-point).normalize();
}
