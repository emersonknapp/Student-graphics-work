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
	Material(vec3 a, vec3 d, vec3 s, float r, int p);
	vec3 ka; //ambient
	vec3 kd; //diffuse
	vec3 ks; //specular
	float kr; //reflection coefficient
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
	void translate(int x, int y, int z) ;
	void rotate(int angle, vec3 u);
	void scale(float xScale, float yScale, float zScale);
	void scale(vec3 s);
	mat3 dehomogenize(mat4 t);
	vec3 dehomogenize(vec4 v);
	
	virtual bool ray_intersect (Ray &, float &, vec3 &)=0; // returns whether ray intersects this object, sets t to proper value

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
	bool ray_intersect (Ray &, float &, vec3 &);
	Ray generate_ray(float x, float y);
};

class Sphere : public Renderable {
// inherits tmat from Renderable
public:
	float radius;
	vec4 base;
	
	Sphere (float a);	
	bool ray_intersect ( Ray &, float &, vec3 &);
};

class Triangle : public Renderable {
public:
	//vertices
	vec4 v1, v2, v3;
	Triangle (vec4 a, vec4 b, vec4 c);
	
	bool ray_intersect ( Ray &, float &, vec3 & );
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