// Simple OpenGL example for CS184 sp08 by Trevor Standley, modified from sample code for CS184 on Sp06
#include <vector>
#include <iostream>
#include <fstream>
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

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define FRAMERATE 10
#define EPSILON 0.15
#define DEBUG false
#define BITSPERPIXEL 24

using namespace std;


//****************************************************
// Classes
//****************************************************

class Viewport {
public:
	Viewport () {
		w = 0;
		h = 0;
	}
	Viewport (int width, int height) {
		w = width;
		h = height;
	}
	int w;
	int h;
};

class Material {
public:
	Material() {
		ka = vec3(0,0,0);
		kd = vec3(0,0,0);
		ks = vec3(0,0,0);
		sp = 0;
		toonResolution = 1;
		bToonShade = false;
	}
	Material(vec3 a, vec3 d, vec3 s, int p) {
		ka = a;
		kd = d;
		ks = s;
		sp = p;
		toonResolution = 1;
		bToonShade = false;
	}
	vec3 ka;
	vec3 kd;
	vec3 ks;
	int sp;
	int toonResolution;
	bool bToonShade;
};

class PLight {
public:
	PLight(vec3 p, vec3 i) {
		pos = p;
		intensity = i;
	}
	vec3 pos;
	vec3 intensity;
};

class DLight {
public:
	DLight(vec3 d, vec3 i) {
		dir = d;
		intensity = i;
	}
	vec3 dir;
	vec3 intensity;	
};

struct FileWriter {
	bool drawing;
	char * fileName;
} fileWriter;
	


//****************************************************
// Global Variables
//****************************************************
Viewport			viewport;
Material 			material;
vector<PLight>		plights;
vector<DLight>		dlights;
int					sphereRadius;


void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glVertex2f(x+0.5, y+0.5);
}

vec3 multiplyVectors(vec3 a, vec3 b) {
	vec3 c;
	c[0] = a[0] * b[0];
	c[1] = a[1] * b[1];
	c[2] = a[2] * b[2];
	return c;
}

void quitProgram() {
	FreeImage_DeInitialise();
	exit(0);
}

void printScreen(char * name) {
	//bitmap holds FreeImage Pixels
	FIBITMAP* bitmap = FreeImage_Allocate(viewport.w, viewport.h, BITSPERPIXEL);
	if (!bitmap) exit(1);
	RGBQUAD color;
	
	//pRGB holds openGL pixel output
	unsigned char *pRGB = new unsigned char [3* (viewport.w+1) * (viewport.h+1) + 3];
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, viewport.w, viewport.h, GL_RGB, GL_UNSIGNED_BYTE, pRGB);

	for (int i=0; i<viewport.w*viewport.h; i++) {
		int index = i*3;

		int x = i%viewport.w;
		int y = (i - i%viewport.w)/viewport.w;
		color.rgbRed = pRGB[index];
		color.rgbGreen = pRGB[index+1];
		color.rgbBlue = pRGB[index+2];
		FreeImage_SetPixelColor(bitmap, x, y, &color);
	}

	FreeImage_Save(FIF_PNG, bitmap, name, 0);
	cout << "Image successfully saved to " << name << endl;
}

//***************************************************
// function that does the actual drawing
//***************************************************
vec3 shade(vec3 pos) {
	vec3 normal = vec3(pos);
	normal.normalize();
	vec3 color = vec3(0,0,0); //Default black

	
	vec3 viewVector = vec3(0,0,1);
	
	for (int i=0; i<plights.size(); i++) {
		vec3 lightColor = plights[i].intensity;
		vec3 lightVector = (plights[i].pos * sphereRadius) - pos;
		lightVector.normalize();
		vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
		//Ambient term
		color += multiplyVectors(lightColor, material.ka);
		//Diffuse term
		color += multiplyVectors(material.kd, lightColor)*max(lightVector*normal, 0.0);
		//Specular term
		color += multiplyVectors(material.ks, lightColor)*pow(max(reflectionVector*viewVector, 0.0), material.sp);
		if (material.bToonShade && (viewVector*normal < EPSILON)) {
			color = vec3(0, 0, 0);
		}
	}
	for (int i=0; i<dlights.size(); i++) {
		vec3 lightColor = dlights[i].intensity;
		vec3 lightVector = vec3(0,0,0) - dlights[i].dir*sphereRadius;
		lightVector.normalize();
		vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
		//Ambient term
		color += multiplyVectors(lightColor, material.ka);
		//Diffuse term
		color += multiplyVectors(material.kd, lightColor)*max(lightVector*normal, 0.0);
		//Specular term
		color += multiplyVectors(material.ks, lightColor)*pow(max(reflectionVector*viewVector, 0.0), material.sp);
		if (material.bToonShade && (viewVector*normal < EPSILON)) {
			color = vec3(0, 0, 0);
		}
	}
	if (material.bToonShade) {
		int res = material.toonResolution; //(255/res)^3 colors
		float r = color[0];
		float g = color[1];
		float b = color[2];
		int newr = int(r*255);
		int newg = int(g*255);
		int newb = int(b*255);
		newr -= newr%res;
		newg -= newg%res;
		newb -= newb%res;
		r = newr/255.0;
		g = newg/255.0;
		b = newb/255.0;
		color = vec3(r, g, b);
	}
	
	return color;
}

void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	int radius = min(viewport.w, viewport.h)-2;
	sphereRadius = radius;
	
	glBegin(GL_POINTS);
	
	//Draw shaded sphere
	vec3 zero = vec3(0,0,0);
	for (int i = -radius; i <= radius; i++) {
		int width = (int)(sqrt((float)(radius*radius-i*i)) + 0.5f);
		for (int j = -width; j <= width; j++) {
			float za = radius*radius - (j*j + i*i);
			if (za > 0) {
				float z = sqrt(za);
				vec3 normal = vec3(j, i, z);
				vec3 color = shade(normal);
				setPixel(j, i, color[0], color[1], color[2]);
			}

		}
	}
	

	glEnd();
	
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
	
	if (fileWriter.drawing) {
		printScreen(fileWriter.fileName);
		quitProgram();
		
	}
}

//Reshape the viewport if the window is resized
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	
	glViewport(0,0, w, h);// sets the rectangle that will be the window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();				// loading the identity matrix for the screen
	
	gluOrtho2D(-w, w, -h, h);

}

//Deals with normal keydowns
void processNormalKeys(unsigned char key, int x, int y) {
	//escape, q quit
	if (key == 27 || key == 'q' || key==32) {
		quitProgram();
	} else if (key == 't') {
		bool ts = !material.bToonShade;
		material.bToonShade = ts;
		glClearColor(ts, ts, ts, 0);
	} else if (key == 'y') {
		material.toonResolution = min(material.toonResolution+4, 255);
	} else if (key == 'r') {
		material.toonResolution = max(material.toonResolution-4, 1);
	} else if (key >= '0' && key <= '9') {
		char name[9];
		strcpy(name, "pic");
		name[3] = key;
		name[4] = '\0';
		strcat(name, ".png");
		printScreen(name);
	}
}

//Deals with normal keyups
void processNormalKeyups(unsigned char key, int x, int y) {

}

//****************************************************
// sets the window up
//****************************************************
void initScene(){
	if (material.bToonShade) {
		glClearColor(1, 1, 1, 1);
	} else {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	}
	myReshape(viewport.w,viewport.h);
	if (DEBUG)
		cout << "Scene initialized" << endl;
}


void processArgs(int argc, char* argv[]) {
	for (int i=1; i<argc; i++) {
		string arg = argv[i];
		if (arg=="-ka") {
			float r = atof(argv[++i]);
			float g = atof(argv[++i]);
			float b = atof(argv[++i]);
			material.ka = vec3(r, g, b);
		} else if (arg=="-kd") {
			float r = atof(argv[++i]);
			float g = atof(argv[++i]);
			float b = atof(argv[++i]);
			material.kd = vec3(r,g,b);
		} else if (arg=="-ks") {
			float r = atof(argv[++i]);
			float g = atof(argv[++i]);
			float b = atof(argv[++i]);
			material.ks = vec3(r,g,b);
		} else if (arg=="-sp") {
			int sp = atoi(argv[++i]);
			material.sp = sp;
		} else if (arg=="-pl") {
			if (plights.size() < 5) {
				float x = atof(argv[++i]);
				float y = atof(argv[++i]);
				float z = atof(argv[++i]);
				float r = atof(argv[++i]);
				float g = atof(argv[++i]);
				float b = atof(argv[++i]);
				plights.push_back(PLight(vec3(x,y,z), vec3(r,g,b)));
			} else {
				i +=  6;
			}
		} else if (arg=="-dl") {
			if (dlights.size() < 5) {
				float x = atof(argv[++i]);
				float y = atof(argv[++i]);
				float z = atof(argv[++i]);
				float r = atof(argv[++i]);
				float g = atof(argv[++i]);
				float b = atof(argv[++i]);
				dlights.push_back(DLight(vec3(x,y,z), vec3(r,g,b)));
			} else {
				i+=6;
			}
		} else if (arg=="-ts") {
			material.bToonShade = true;
			material.toonResolution = atoi(argv[++i]);
		} else if (arg=="-pr") {
			fileWriter.drawing = true;
			fileWriter.fileName = argv[++i];
		}
	}
}
//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	srand((unsigned)time(NULL));
	
	//Initialize FreeImage library
	FreeImage_Initialise();
	cout << "FreeImage " << FreeImage_GetVersion() << endl;
	cout << FreeImage_GetCopyrightMessage() << endl;
	
  	//This initializes glut
	material = Material();
	processArgs(argc, argv);
  	glutInit(&argc, argv);
	
  	//This tells glut to use a double-buffered window with red, green, and blue channels 
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	viewport = Viewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	
  	//The size and position of the window
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(-1, -1);
  	glutCreateWindow("Phong Illumination Model");
	
  	initScene();							// quick function to set up scene
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeyups);
	glutIdleFunc(glutPostRedisplay);
  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
  	return 0;
}








