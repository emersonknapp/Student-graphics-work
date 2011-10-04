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
// Global Variables
//****************************************************
Viewport			viewport;
vector<PLight*>		plights;
vector<DLight*>		dlights;
vector<Renderable*>	renderables;
Camera				camera;
FileWriter			fileWriter;
Scene				scene;



//****************************************************
// Helper Functions
//****************************************************

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

void Error(string msg) {
	cout << msg << endl;
	exit(1);
}

void quitProgram() {
	//Make sure to delete stuff that was created using new.
	for (int i=0; i < renderables.size(); i++) delete renderables[i];
	FreeImage_DeInitialise();
	exit(0);
}

void FileWriter::printScreen() {
	const char* name = fileName.c_str();
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
		Ray lightCheck = Ray(hitPoint, plights[i]->pos - hitPoint);
		int t;
		Material material;
		for (int k = 0; k < renderables.size() ; k++ ) {
			t = 1;
			normal = vec4(0,0,0,0);

			renderables[k]->ray_intersect(lightCheck,t,normal);
			material = renderables[k]->material;
		}
		vec4 intersection = ray.pos + t * ray.dir;
		if (t == 1) { // then shade. if t != 1, then the light is blocked by another object
			vec3 normal = vec3(normal[0],normal[1],normal[2]);
			vec3 lightColor = plights[i]->intensity;
			vec3 lightVector = plights[i]->pos - intersection; // this pos is the intersection point on the sphere
			lightVector.normalize();
			vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
			//Ambient term
			color += multiplyVectors(lightColor, material.ka);
			//Diffuse term
			color += multiplyVectors(material.kd, lightColor)*max(lightVector*normal, 0.0);
			//Specular term
			color += multiplyVectors(material.ks, lightColor)*pow(max(reflectionVector*vec3(ray.pos[0],ray.pos[1],ray.pos[2]), 0.0), material.sp);
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


//***************************************************
// Function what actually draws to screen
//***************************************************
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
			Ray r = Ray(camera.pos,vec4(i,j,0,1) - camera.pos);
			int t=0;
			vec4 normal;
			for (int k = 0; k < renderables.size() ; k++ ) {
				normal = vec4(0,0,0,0);
				//TODO this next line causes a segfault at runtime. I think it has something to do with the Ray r
				renderables[k]->ray_intersect(r,t,normal);
			}
			vec4 intersection = r.pos * t * r.dir; // at this point, t is minimum
			vec3 color = shade(r, intersection, normal, 1); // recursionDepth = 1 for debug purposes
			setPixel(i, j, color[0], color[1], color[2]);
			
		}
	}
		
	glEnd();
		
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
	
	if (fileWriter.drawing) {
		if (DEBUG) cout << "Completed render! Outputting to file." << endl;
		fileWriter.printScreen();
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
		string name = "pic";
		name += key;
		name += ".png";
		fileWriter.fileName = name;
		fileWriter.printScreen();
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
		mat4 tranlation;
		mat4 scale;
		mat4 rotate;
		
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
					Sphere* sph = new Sphere(r);
					sph->translate(0,0,150);
					renderables.push_back(sph);
				} else {
					Error("Sphere object needs radius.");
				}
				if (DEBUG) cout << "Added sphere of radius " << r << " to scene." << endl;
				
			} else if (word == "tri") { //triangle x1 y1 z1 x2 y2 z2 x3 y3 z3
				vec4 vertices[3];
				for (int i=0; i<3; i++) {
					int v[3];
					for (int j=0; j<3; j++) {
						iss >> word;
						if (iss) {
							v[j]=atoi(word.c_str());
						} else {
							Error("Not enough arguments to triangle.");
						}
					}
					vertices[i] = vec4(v[0], v[1], v[2], 1);
				}
				Triangle* tri = new Triangle(vertices[0], vertices[1], vertices[2]);
				renderables.push_back(tri);
				if (DEBUG) cout << "Added triangle to scene." << endl;
			} else if (word == "camera") { //camera viewdir upvec fov
				camera = Camera();
			} else if (word == "print") { //print outputfile
				fileWriter.drawing = true;
				iss >> word;
				if (iss) {
					fileWriter.fileName = word;
				} else {
					fileWriter.fileName = "out.png";
				}
			} else if (word == "translate") { //translate x y z
				int x = 0, y=0, z=0;
			} else if (word == "rotate") { //rotate theta vec

			} else if (word == "scale") { //scale x y z

			} else if (word == "mat") { //mat ka kd ks kr sp

			} else if (word == "pl") { //pointlight x y z r g b
				vec4 pos;
				vec3 color;
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						pos[i] = atoi(word.c_str());
					} else Error("Not enough arguments to PointLight");
				}
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						color[i] = atoi(word.c_str());
					} else Error("Not enough arguments to PointLight");
				}
				pos[3] = 0;
				PLight* p = new PLight(pos, color);
				plights.push_back(p);
				if (DEBUG) cout << "Added point light to scene." << endl;
			} else if (word == "dl") { //directionalight x y z

			} else {
				Error("Unrecognized object " + word);
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

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeyups);
	glutIdleFunc(glutPostRedisplay);
  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
  	return 0;
}








