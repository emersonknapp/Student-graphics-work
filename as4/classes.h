#ifndef _CLASSES_H
#define _CLASSES_H

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

#include "algebra3.h"
#include "FreeImage.h"



//
// Basic class definition for anything we're going to render or apply transformations to
// Call the map/rotate/scale transformation methods in the order you want to apply the transformations.
// The resulting transformation matrix will be stored in tmat, which has transpose() and inverse() methods
// for the necessary operations
//

using namespace std;

class Viewport {
public:
	Viewport ();
	Viewport (int width, int height);
	int w;
	int h;
};

class PLight {
public:
	PLight(vec4, vec3);
	vec4 pos;
	vec3 intensity;
};

class DLight {
public:
	DLight(vec4 , vec3 );
	vec4 dir;
	vec3 intensity;	
};

class Material {
public:
	Material();
	Material(vec3 a, vec3 d, vec3 s, vec3 r, int p);
	vec3 ka; //ambient
	vec3 kd; //diffuse
	vec3 ks; //specular
	vec3 kr; //reflection coefficient
	int sp; //specular power
};

class Ray {
public:
	vec4 pos;
	vec4 dir;
	float t_min, t_max;
	Ray();
	Ray(vec4 a, vec4 b);	
};

class Renderable {
public:
	mat4 tmat;
	mat4 imat;
	Material material;
	// constructors
	Renderable () ;
	// methods
	void translate(vec3 t);
	void translate(float x, float y, float z) ;
	void rotate(float, float, float);
	void rotate(vec3);
	void scale(float xScale, float yScale, float zScale);
	void scale(vec3 s);
	virtual vec4 normal(vec4)=0;
	
	mat3 dehomogenize(mat4 t);
	
	virtual float ray_intersect (Ray)=0; // returns whether ray intersects this object, sets t to proper value

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
	float ray_intersect (Ray);
	Ray generate_ray(float x, float y);
	vec4 normal(vec4);
	
};

class Sphere : public Renderable {
// inherits tmat from Renderable
public:
	
	Sphere ();	
	float ray_intersect ( Ray);
	vec4 normal(vec4);
};

class Triangle : public Renderable {
public:
	//vertices
	vec4 v1, v2, v3;
	vec3 norm;
	Triangle (vec4 a, vec4 b, vec4 c);
	
	float ray_intersect ( Ray);
	vec4 normal(vec4);
};

class Scene {
public:
	Scene();
	~Scene();
	vector<Renderable*> renderables;
	vector<PLight*> plights;
	vector<DLight*> dlights;
	vector<Camera*> cameras;
	int activeCamera;	
};

class FileWriter {
public:
	bool drawing;
	string fileName;
	void printScreen();
};



#endif