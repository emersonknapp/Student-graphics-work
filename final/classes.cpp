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
	jittery = false;
}
Viewport::Viewport (int width, int height) {
	w = width;
	h = height;
	aliasing = 0;
	jittery = false;
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

vector<Photon*> PLight::emitPhotons() {
	vector<Photon*> photonCloud;
	//TODO: send out photons in random directions, currently iterating through spherical coordinates which leads to a bias towards the poles
	float stepsize = 0.1;
	//iterate through longitude and latitude of sphere around pointlight
	for (float longi=0.0; longi<=2.0; longi+=stepsize) {
		for (float lati=-0.5; lati<=0.5; lati+=stepsize) {
			//forming a sphere of photons around the point light
			float phi = longi*3.1415;
			float theta = lati*3.1415;
			vec4 photonDir = vec4(cos(phi)*sin(theta),sin(phi),cos(phi)*sin(theta),0);
			Photon* photon = new Photon(v,photonDir,intensity);
			photonCloud.push_back(photon);
		}
	}
	return photonCloud;
}

vec4 DLight::lightVector(vec4 origin) {
	return -v;
}

vector<Photon*> DLight::emitPhotons() {
	vector<Photon*> photonCloud;
	//TODO: implement emitPhotons() for directional lights!!!
	return photonCloud;
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



bool AABB::intersect(AABB* other) {
	//Idea: just make 16 corner points, check each if it lies within the other box.
	//	Look online for more efficient implementation. Until then, this.
	//Make 8 corner points for this
	vec3 box[8];
	vec3 otherbox[8];
	int bit;
	for (int i=0; i<8; i++) {
		for (int j=0; j<3; j++) {
			bit = (i>>j)%2;
			box[i][j] = (1-bit)*mins[j] + bit*maxes[j];
			otherbox[i][j] = (1-bit)*other->mins[j] + bit*other->maxes[j];
		}
		if (other->intersect(box[i])) return true; //True if I contain a corner within the other box
		if (this->intersect(otherbox[i])) return true; //or if it contains a corner within me
	}
		
	return false;
}

bool AABB::intersect(vec3 point) {
	bool inside =
		point[0] >= mins[0] && point[0] <= maxes[0] &&
		point[1] >= mins[1] && point[1] <= maxes[1] &&
		point[2] >= mins[0] && point[2] <= maxes[2];
	return inside;
}

//Makes this aabb the concatenation of itself and OTHER
void AABB::concat(AABB* other) {
	for (int i=0; i<3; i++) {
		mins[i] = min(mins[i], other->mins[i]);
		maxes[i] = max(maxes[i], other->maxes[i]);
	}
}

void AABB::concat(vec3 other) {
	for (int i=0; i<3; i++) {
		mins[i] = min(mins[i], other[i]);
		maxes[i] = max(maxes[i], other[i]);
	}
}

void AABB::print(int indent) {
	cout << string(indent*2, ' ') << "Bounding volume: " << endl;
	cout << string(indent*2, ' ') << mins << maxes << endl;
}
