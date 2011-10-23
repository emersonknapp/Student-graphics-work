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

enum {TRIS, QUADS};



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

class Light {
public:
	vec3 intensity;
	vec4 v;
	virtual vec4 lightVector(vec4) = 0;
};

class LocalGeo {
public:
	vec3 pos;
	vec3 dir;
};

class PLight: public Light {
public:
	PLight(vec4, vec3);
	vec4 lightVector(vec4);
};

class DLight: public Light {
public:
	DLight(vec4 , vec3 );
	vec4 lightVector(vec4);
};

class Material {
public:
	Material();
	Material(vec3 d, vec3 s, vec3 r, int p);

	vec3 kd; //diffuse
	vec3 ks; //specular
	vec3 kr; //reflection coefficient
	int sp; //specular power
};

class Camera {	
public:
	vec4 pos;
	vec4 UL, UR, LL, LR;
	Camera();	
};


#endif