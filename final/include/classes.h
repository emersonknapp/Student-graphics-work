#ifndef _CLASSES_H
#define _CLASSES_H

#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <climits>

#include <time.h>
#include <math.h>

#include "util.h"
#include "algebra3.h"
#include "FreeImage.h"

#define WIGGLE .005
//#define T_MAX 1000

using namespace std;
class Renderable;
class Photon;
typedef vector<Renderable*>::iterator rendIt; //Renderable Pointer Iterator
typedef vector<Photon*>::iterator photIt;


class Ray {
public:
	vec4 pos;
	vec4 dir;
	float t_min, t_max;
	vector<float> ristack;
	Ray();
	Ray(vec4 a, vec4 b);
};



class AABB {
public:
	AABB();
	AABB(vec3, vec3);
	void concat(AABB*);
	void concat(vec3);
	bool rayIntersect(Ray);
	bool intersect(AABB*);
	bool intersect(vec3);
	void print(int);
	vec3 mins;
	vec3 maxes;
};

class Photon : public Ray {
public:
	vec3 color;
	Photon();
	Photon(vec4 a, vec4 b, vec3 c);
};

class Viewport {
public:
	Viewport ();
	Viewport (int width, int height);
	int w;
	int h;
	int aliasing;
	bool jittery; 
	bool photons; 
	int photonsPerLight;
	float gatherEpsilon;
	int causticPhotonsPerLight;
};

class Light {
public:
	vec3 intensity;
	vec4 pos;
	float power;
	virtual vec4 lightVector(vec4) = 0;
	virtual void generatePhotons(vector<Photon*>&, int numPhotons, AABB* s) = 0;
};

class PLight: public Light {
public:
	PLight(vec4, vec3);
	PLight(vec4, vec3, float);
	vec4 lightVector(vec4);
	void generatePhotons(vector<Photon*>&, int numPhotons, AABB* s);
};

class DLight: public Light {
public:
	DLight(vec4 , vec3 );
	vec4 lightVector(vec4);
	void generatePhotons(vector<Photon*>&, int numPhotons, AABB* s);
};

class Texture {
// class for texture maps
public:
	bool exists;	
	float width;
	float height;
	FIBITMAP *txt;
	Texture();
	Texture(const char* fn);
	vec3 getColor(float x, float y);	
};

class Material {
public:
	Material();
	Material(vec3 a, vec3 d, vec3 s, vec3 r, int p);
	vec3 ka; //ambient
	vec3 kd; //diffuse
	vec3 ks; //specular
	vec3 kr; //reflection coefficient
	float ri; //refractive index
	int sp; //specular power
	Texture texture;
};

//
// Basic class definition for anything we're going to render or apply transformations to
// Call the map/rotate/scale transformation methods in the order you want to apply the transformations.
// The resulting transformation matrix will be stored in tmat, which has transpose() and inverse() methods
// for the necessary operations
//

class Renderable {
public:
	mat4 tmat;
	mat4 imat;
	mat4 rotmat;
	mat4 transmat;
	mat4 scalemat;
	vec3 center;
	Material material;
	AABB* aabb;
	virtual AABB* makeAABB();
	// constructors
	Renderable ();
	// methods
	void translate(vec3 t);
	void translate(float x, float y, float z) ;
	void rotate(float, float, float);
	void rotate(vec3);
	void scale(float xScale, float yScale, float zScale);
	void scale(vec3 s);
	virtual vec4 normal(vec4)=0;
	virtual vec3 textureColor(vec4)=0;
	virtual vec4 randomSurfacePoint()=0;
	
	mat3 dehomogenize(mat4 t);
	
	virtual float rayIntersect (Ray)=0; // returns whether ray intersects this object, sets t to proper value

};

class Camera : public Renderable {
	// needs to keep track of its position (starts at 0,0,-1) by default.
	// we ask the camera to generate a ray based on an input between (1,1) (1,-1) (-1,-1) (-1,1)
	// we then scale this point to the size of the viewport, then run the regular camera transformations on it then generate the ray.
	
public:
	vec4 pos;
	vec4 UL, UR, LL, LR;
	Camera();	
	//Ray generate_ray();
	float rayIntersect (Ray);
	Ray generate_ray(float x, float y);
	vec4 normal(vec4);
	vec3 textureColor(vec4);	
	vec4 randomSurfacePoint();
};

class Sphere : public Renderable {
// inherits tmat from Renderable
public:
	
	Sphere ();	
	float rayIntersect ( Ray);
	AABB* makeAABB();
	vec4 normal(vec4);
	vec3 textureColor(vec4);
	vec4 randomSurfacePoint();
};

class Triangle : public Renderable {
public:
	//vertices
	vec4 v1, v2, v3;
	vec3 vt1, vt2, vt3; //texture vertices
	vec4 vn1, vn2, vn3;
	vec3 norm;
	Triangle (vec4 a, vec4 b, vec4 c);
	Triangle (vec4 a, vec4 b, vec4 c, vec3 g, vec3 h, vec3 i, vec4 m, vec4 n, vec4 o);	
	float rayIntersect ( Ray);
	AABB* makeAABB();
	vec4 normal(vec4);
	vec3 textureColor(vec4);
	vec4 randomSurfacePoint();
};



#endif
