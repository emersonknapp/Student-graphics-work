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
	photoooooooons = false; 
}
Viewport::Viewport (int width, int height) {
	w = width;
	h = height;
	aliasing = 0;
	jittery = false;
	photoooooooons = false;
}

PLight::PLight(vec4 p, vec3 i) {
	pos = p;
	intensity = i;
}

DLight::DLight(vec4 d, vec3 i) {
	pos = d;
	intensity = i;
}

vec4 PLight::lightVector(vec4 origin) {
	return pos-origin;
}

void PLight::generatePhotons(vector<Photon*>& photonCloud, int numPhots, AABB* s) {
	
	for (int i=0; i<numPhots; i++) {
		vec3 photonDir = randomSpherePoint(); 
//		cout << photonDir << endl;
		Photon* photon = new Photon(pos, vec4(photonDir,0), intensity);
		photonCloud.push_back(photon);
	}
	
}

vec4 DLight::lightVector(vec4 origin) {
	return -pos;
}

void DLight::generatePhotons(vector<Photon*>& photonCloud, int numPhots, AABB* s) {
	float u,v;
	// centered at lower right corner of plane
	vec4 lowerRight;
	for (int i = 0; i < 3; i++) {
		if (pos[i]>=0) lowerRight[i] = s->mins[i];
		else if (pos[i]<0) lowerRight[i] = s->maxes[i];
	}
	lowerRight[3] = 1;
	vec4 vDir = vec4( max(1.0,abs(pos[0])) - abs(pos[0]),
					  max(1.0,abs(pos[1])) - abs(pos[1]),
					  max(1.0,abs(pos[2])) - abs(pos[2]),
					  0
					);
	vec4 uDir = vec4( pos[0],
					  pos[1],
					  pos[2],
					  0
					);
	float uScale, vScale; //TODO: scale u,v by these amounts
	for (int i=0; i<numPhots; i++) {
		u = rand01();
		v = rand01();
		// want to do u * ("x" dir), v * ("y" dir) to get the origin of the photonCloud
		vec4 photonPos = lowerRight+u*uDir + v*vDir;
		photonPos = lowerRight + u * vec4(1,0,0,0) + v * vec4(0,1,0,0);
		vec4 photonDir = pos.normalize();
		Photon* photon = new Photon(photonPos, photonDir, intensity);
		photonCloud.push_back(photon);
	}
	
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
	for (int i=0; i<3; i++) {
		if (mins[i] > other->maxes[i] or other->mins[i] > maxes[i]) return false;
	}
	return true;
}

bool AABB::intersect(vec3 point) {
	bool inside =
		point[0] >= mins[0] && point[0] <= maxes[0] &&
		point[1] >= mins[1] && point[1] <= maxes[1] &&
		point[2] >= mins[2] && point[2] <= maxes[2];
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
