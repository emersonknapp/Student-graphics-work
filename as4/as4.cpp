// Simple OpenGL example for CS184 sp08 by Trevor Standley, modified from sample code for CS184 on Sp06
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
#include "classes.h"
#include "errors.h"

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAMERATE 10
#define EPSILON 0.15
#define DEBUG true
#define BITSPERPIXEL 24
#define MAXRECURSION 1
#define MAXLINE 255

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
vector<PLight>		plights;
vector<DLight>		dlights;
vector<Renderable>	renderables;
Camera				camera;


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
// does phong shading on a point
//***************************************************
vec3 shade(Ray ray, vec4 hitPoint, vec4 normal, int recursionDepth) {

	vec3 color = vec3(0,0,0); //Default black

	//Recursion cutoff check
	if (recursionDepth >= MAXRECURSION) {
		return color;
	}
	//Process:
	//draw ray from the intersection point to each of the point lights (define direction as pointLight.pos - intersection).
	// Find the intersection point t.  If t < 1, then there's something in the way, so just skip that point light. 
	// If t=1, then we're good, so run Phong Shading to get the color of the pixel. Reflect this, with shade() recursive calls
	// don't forget to increase recursionDepth!
	
	
	
	//Loop through point lights
	for (int i=0; i<plights.size(); i++) {
		//check if there's shadow
		Ray lightCheck = Ray(hitPoint,plights[i].pos - hitPoint);
		for (int k = 0; k < renderables.size() ; k++ ) {
			t = 0;
			normal = vec4(0,0,0,0);
			renderables[k].ray_intersect(r,t,normal)
		}
		if (t == 1) { // then shade. if t != 1, then the light is blocked by another object
			vec3 lightColor = plights[i].intensity;
			vec3 lightVector = plights[i].pos - pos;		
			lightVector.normalize();
			vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
			//Ambient term
			color += multiplyVectors(lightColor, material.ka);
			//Diffuse term
			color += multiplyVectors(material.kd, lightColor)*max(lightVector*normal, 0.0);
			//Specular term
			color += multiplyVectors(material.ks, lightColor)*pow(max(reflectionVector*viewVector, 0.0), material.sp);
		}
	}


	/*
	//Loop through directional lights
	for (int i=0; i<dlights.size(); i++) {
		vec3 lightColor = dlights[i].intensity;
		vec3 lightVector = vec3(0,0,0) - dlights[i].dir;
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
	*/
	
	return color;
}

void myDisplay() {

	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	glBegin(GL_POINTS);
	
	//TODO: Loop through each sample that we want to take (at first just each pixel)
		//We cast a ray from the camera towards each sample
		//then call shade to calculate the color of that sample
	for (int i = -viewport.w; i<viewport.w; i++) {
		for (int j = -viewport.h; j<viewport.h; j++) {
			//It'll probably be nicer if we ask the camera for the ray,
			//then it can transform it into worldspace for us before we even see it.
			//Ray r = camera.generateRay(i, j);
			Ray r = Ray(camera.pos,vec4(i,j,0,1) - camera.pos);
			int t;
			vec4 normal;
			for (int k = 0; k < renderables.size() ; k++ ) {
				t = 0;
				normal = vec4(0,0,0,0);
				if (renderables[k].ray_intersect(r,t,normal)) {
					vec4 intersection = r.pos * t * r.dir;
					// run shader
				}
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	myReshape(viewport.w,viewport.h);
	if (DEBUG) cout << "Scene initialized" << endl;
}


void processArgs(int argc, char* argv[]) {
	
	for (int i=1; i<argc; i++) {
		Material material;
		mat4 tmat;
		
		string arg = argv[i];
		
		ifstream inFile(arg.c_str());
		char line[MAXLINE];
		while (inFile.getline(line, MAXLINE)) {
			string s(line);
			istringstream iss(s);
			string word; //Holds current word from the line input
			iss >> word;
			if (word == "sph") { //Parse a sphere
				int r;
				iss >> word;
				if (iss) {
					r = atoi(word.c_str());
					Sphere s(r);
					renderables.push_back(s);
					if (DEBUG) cout << "Parsed sphere of radius " << r << endl;
				} else {
					tooFewArgumentsError("Sphere object needs radius.");
				}
			} else if (word == "tri") { //Parse a triangle
				
			} else if (word == "camera") { //Initialize the camera
				
			} else if (word == "print") { //Specify output file, default "out.png"
				
			} else if (word == "translate") { //specify translation

			} else if (word == "rotate") { //specify rotation

			} else if (word == "scale") { //specify scale

			} else if (word == "mat") { //specify a material

			} else {
				continue;
			}
		}
		
		inFile.close();

	}
	
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	srand((unsigned)time(NULL));
	
	//Initialize FreeImage library
	FreeImage_Initialise();
	//cout << "FreeImage " << FreeImage_GetVersion() << endl;
	//cout << FreeImage_GetCopyrightMessage() << endl;
	
  	//This initializes glut
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
	// initialize sample models so we don't have to worry about CLI yet
	Sphere s1 = Sphere(min(viewport.w, viewport.h)-2);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeyups);
	glutIdleFunc(glutPostRedisplay);
  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
  	return 0;
}








