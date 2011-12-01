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
}	

Ray::Ray(vec4 a, vec4 b) {
	pos = a;
	dir = b;
	dir.normalize();
}	

Photon::Photon() : Ray() {
	color = vec3(0,0,0);
}

Photon::Photon(vec4 a, vec4 b, vec3 c) : Ray(a, b) {
	color = c;
}

AABB* Photon::makeAABB() {
	aabb = new AABB(pos, pos);
	return aabb;
}

Texture::Texture() {
	exists = false;
}

Texture::Texture(const char* fn) {
	txt = FreeImage_Load(FIF_PNG, fn, PNG_DEFAULT);
	width = FreeImage_GetWidth(txt);
	height = FreeImage_GetHeight(txt);
	exists = true;
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
	aliasing = 0;
	jaliasing = 0;
}
Viewport::Viewport (int width, int height) {
	w = width;
	h = height;
	aliasing = 0;
	jaliasing = 0;
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

void PLight::emitPhotons(Scene* scene) {
	/*To Harry: if you put a statement TODO anywhere,
		you can run 
		grep -R -n "TODO" .
		and it will display all TODOs in the directory, plus line numbers and file names
		delete this once you've read it
	*/ 
	int renderableIndex;
	float t;
	bool hasHit;
	//TODO: send out photons in random directions, currently iterating through spherical coordinates which leads to a bias towards the poles
	float stepsize = 0.1;
	//iterate through longitude and latitude of sphere around pointlight
	for (float longi=0.0; longi<=2.0; longi+=stepsize) {
		for (float lati=-0.5; lati<=0.5; lati+=stepsize) {
			//calculate intersections against renderables, store photon into scene->photons, reflection photons?
			float phi = longi*3.1415;
			float theta = lati*3.1415;

			vec4 photonDir = vec4(cos(phi)*sin(theta),sin(phi),cos(phi)*sin(theta);
			Ray r = Ray(v,photonDir,0);

			renderableIndex=-1;
			t = T_MAX;
			hasHit = false;

			hasHit = scene->rayIntersect(r, t, renderableIndex);

			if (hasHit) {
				Photon* photon = new Photon(v,photonDir,intensity);
				scene->photons.push_back(photon);
			}
	}
	return;
}

vec4 DLight::lightVector(vec4 origin) {
	return -v;
}

void DLight::emitPhotons(Scene* scene) {
	//TODO: IMPLEMENT...CAPS!!!
	return;
}

AABB::AABB() {
	mins = vec3(INT_MAX, INT_MAX, INT_MAX);
	maxes = vec3(INT_MIN, INT_MIN, INT_MIN);
}

AABB::AABB(vec3 n, vec3 x) {
	mins = n;
	maxes = x;
}

bool AABB::rayIntersect(Ray r) {
	vec3 raydir = r.dir.dehomogenize();
	vec3 raypos = r.pos.dehomogenize();
	//tvalues at which boxmins are hit
	vec3 tmin;
	//tvalues at which boxmaxes are hit
	vec3 tmax;
	
	float t1;
	float t2;
	float tnear = INT_MIN;
	float tfar = INT_MAX;
	
	for (int i=0; i<3; i++) {
		if (raydir[i] == 0) {
			if (!(raypos[i] >= mins[i] && raypos[i] <= maxes[i])) {
				return false;
			}
		} else {
			t1 = (mins[i]-raypos[i])/raydir[i];
			t2 = (maxes[i]-raypos[i])/raydir[i];
			if (t1 > t2) swap(t1, t2);
			if (t1 > tnear) tnear = t1;
			if (t2 < tfar) tfar = t2;
			if (tnear > tfar) return false;
			if (tfar < 0) return false;
		}
	}
	
	return true;
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
