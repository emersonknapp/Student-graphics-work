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
#define SCREEN_WIDTH 30
#define SCREEN_HEIGHT 30
#define FRAMERATE 10
#define EPSILON 0.005
#define DEBUG true
#define BITSPERPIXEL 24
#define T_MAX 400

#define MAXRECURSION 3
#define MAXLINE 255

using namespace std;

//****************************************************
// Global Variables
//****************************************************
Viewport			viewport;
vector<PLight*>		plights;
vector<DLight*>		dlights;
vector<Renderable*>	renderables;
Camera*				camera;
vec3				ambience;
FileWriter			fileWriter;
Scene				scene;

//****************************************************
// Helper Functions
//****************************************************

vec3 traceRay(Ray, int);

void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glVertex2f(x, y);
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
	for (int i=0; i<plights.size(); i++) delete plights[i];
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
vec3 shade(Ray r, vec4 hitPoint, vec4 norm, int index) {
	vec3 normal = norm.dehomogenize();

	vec3 color = vec3(0,0,0); //Default black

	//Process:
	//draw ray from the intersection point to each of the point lights (define direction as pointLight.pos - intersection).
	// Find the intersection point t.  If t < 1, then there's something in the way, so just skip that point light. 
	// If t=1, then we're good, so run Phong Shading to get the color of the pixel. Reflect this, with shade() recursive calls
	// don't forget to increase recursionDepth!
	
	color += ambience;
	//Loop through point lights
	for (int i=0; i<plights.size(); i++) {
		//check if there's shadow
		Ray lightCheck = Ray(hitPoint, plights[i]->pos - hitPoint);
		float t = T_MAX;
		Material material;
		bool shadePixel = true;
		float newT;

		material = renderables[index]->material;
		vec3 lightColor = plights[i]->intensity;
		vec3 lightVector = (plights[i]->pos - hitPoint).dehomogenize();
		lightVector.normalize();
		vec3 viewVector = (r.pos-hitPoint).dehomogenize();
		viewVector.normalize();
		vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
		reflectionVector.normalize();
		
		for (int j = 0; j < renderables.size(); j++ ) {
			shadePixel = true;
			if((newT=renderables[j]->ray_intersect(lightCheck)) <= lightCheck.dir.length() && newT>0 && index != j) {	
				shadePixel = false;
				break;
			}
		}
		if (shadePixel) {
			//Diffuse term
			color += prod(material.kd, lightColor)*max((lightVector*normal), 0.0);
			//Specular term
			color += prod(material.ks, lightColor)*pow(max(reflectionVector*viewVector, 0.0), material.sp);

		}
		
	}
	
	for (int i=0; i<dlights.size(); i++) {
		vec4 dlightDir = (-dlights[i]->dir);//+vec4(2,2,2,2));
		Ray lightCheck = Ray(hitPoint+norm*EPSILON, dlightDir);
		float t = T_MAX;
		Material material;
		bool shadePixel = true;
		float newT;
		
		material = renderables[index]->material;
		vec3 lightColor = dlights[i]->intensity;

		for (int j = 0; j < renderables.size(); j++ ) {
			shadePixel = true;
			if((newT=renderables[j]->ray_intersect(lightCheck)) < T_MAX && newT>0) {
				//return vec3(1,0,0);
				shadePixel = false;
				break;
			}
		}

		if (shadePixel) {
			vec3 lightVector = vec3(0,0,0) - (dlights[i]->dir).dehomogenize(); //this was vec3(0,0,0) - dlights[i].dir,
			lightVector.normalize();
			vec3 viewVector = (r.pos-hitPoint).dehomogenize();
			viewVector.normalize();
			vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
			//Diffuse
			color += prod(material.kd, lightColor) * max(lightVector*normal, 0.0);
			//Specular
			color += prod(material.ks, lightColor) * pow(max(reflectionVector*viewVector,0.0),material.sp);		
		}
	}
	
	return color;
}

vec3 traceRay(Ray r, int depth) {
	//Recursion cutoff check
	if (depth > MAXRECURSION) {
		return vec3(0,0,0);
	}
	
	float newT;
	int renderableIndex=-1;
	float t = T_MAX;
	bool hasHit = false;
	vec3 color = vec3(0,0,0);
	
	for (int i = 0; i < renderables.size(); i++ ) {
		if((newT=renderables[i]->ray_intersect(r)) < t && newT>0) {	
			hasHit = true;			
			renderableIndex = i;
			t = newT;
		}
	}

	if (hasHit) {
		return vec3(1,1,1);
		vec4 hitPoint = r.pos + t*r.dir;
		vec4 normal = renderables[renderableIndex]->normal(hitPoint);
		color += shade(r, hitPoint, normal, renderableIndex);
		
		vec3 n = normal.dehomogenize();
		vec3 d = r.dir.dehomogenize();
		
		vec3 temp = hitPoint.dehomogenize();
		vec3 refl = temp - (2*(temp*n)*n);
		refl.normalize();
		Ray newray = Ray(hitPoint+EPSILON*normal, refl);
		vec3 kr = renderables[renderableIndex]->material.kr;
		vec3 reflColor = traceRay(newray, depth+1);
		color += prod(kr,reflColor);
		//color = refl;

		return color;
	} else { 
		return vec3(0,0,0);
	}

	
}
//***************************************************
// Function what actually draws to screen
//***************************************************
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	glBegin(GL_POINTS);
	
	for (float x = 0; x < viewport.w; x++) {
		for (float y = 0; y < viewport.h; y++) {
						
			Ray camRay = camera->generate_ray(x/viewport.w,y/viewport.h);			
			
			vec3 color = traceRay(camRay, 0);
			setPixel(x,y,color[0], color[1], color[2]);
		}	
	}
	setPixel(0,0,1,1,1);

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
	
	gluOrtho2D(0, viewport.w, 0, viewport.h);

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
		Material parseMaterial;
		vec3 translation(0,0,0);
		vec3 scale(1,1,1);
		vec3 rotateVec(0,0,0);
		int rotationAmount = 0;
		
		string arg = argv[i];
		
		ifstream inFile(arg.c_str());
		char line[MAXLINE];
		while (inFile.getline(line, MAXLINE)) {
			string s(line);
			istringstream iss(s);
			string word; //Holds current word from the line input
			iss >> word;
			if (word == "ka") {
				float v[3];
				for (int i=0; i < 3; i++) {
					iss >> word;
					if (iss) {
						v[i]=atof(word.c_str());
					} else {
						Error("Not enough arguments to ka.");
					}
				}
				ambience = vec3(v[0],v[1],v[2]);
				if (DEBUG) cout << "added ka = " << ambience << endl;
			}
			else if (word == "kd") {
				float v[3];
				for (int i=0; i < 3; i++) {
					iss >> word;
					if (iss) {
						v[i]=atof(word.c_str());
					} else {
						Error("Not enough arguments to kd.");
					}
				}
				parseMaterial.kd = vec3(v[0],v[1],v[2]);
				if (DEBUG) cout << "added kd = " << parseMaterial.kd << endl;				
			}
			else if (word == "ks") {
				float v[3];
				for (int i=0; i < 3; i++) {
					iss >> word;
					if (iss) {
						v[i]=atof(word.c_str());
					} else {
						Error("Not enough arguments to ks.");
					}
				}
				parseMaterial.ks = vec3(v[0],v[1],v[2]);
				if (DEBUG) cout << "added ks = " << parseMaterial.ks << endl;
			}
			else if (word == "kr") {
				float v[3];
				for (int i=0; i < 3; i++) {
					iss >> word;
					if (iss) {
						v[i]=atof(word.c_str());
					} else {
						Error("Not enough arguments to kr.");
					}
				}
				parseMaterial.kr = vec3(v[0],v[1],v[2]);
			}
			else if (word == "sp") {
				iss >> word;
				if (iss) {
					parseMaterial.sp = atof(word.c_str());
					if (DEBUG) cout << "added sp = " << parseMaterial.sp << endl;
				} else {
					Error("Bad SP");
				}
			}	
			else if (word == "sph") { //Parse a sphere
				float r;
				iss >> word;
				if (iss) {
					r = atof(word.c_str());
					Sphere* sph = new Sphere();
					sph->rotate(rotationAmount, rotateVec);
					sph->scale(r,r,r);
					sph->scale(scale);
					sph->translate(translation);
					sph->material = parseMaterial;
					renderables.push_back(sph);
				} else {
					Error("Sphere object needs radius.");
				}
				if (DEBUG) cout << "Added sphere of radius " << r << " to scene." << endl;
				
			} 
			else if (word == "tri") { //triangle x1 y1 z1 x2 y2 z2 x3 y3 z3
				vec4 vertices[3];
				for (int i=0; i<3; i++) {
					float v[3];
					for (int j=0; j<3; j++) {
						iss >> word;
						if (iss) {
							v[j]=atof(word.c_str());
						} else {
							Error("Not enough arguments to triangle.");
						}
					}
					vertices[i] = vec4(v[0], v[1], v[2], 1);
				}
				Triangle* tri = new Triangle(vertices[0], vertices[1], vertices[2]);
				tri->rotate(rotationAmount, rotateVec);
				tri->scale(scale);
				tri->translate(translation);
				tri->material = parseMaterial;
				renderables.push_back(tri);
				if (DEBUG) cout << "Added triangle to scene." << endl;
			} 
			else if (word == "camera") { //camera
				camera = new Camera();
				camera->translate(translation);
				camera->scale(scale);
				camera->rotate(rotationAmount, rotateVec);
			} 
			else if (word == "print") { //print outputfile
				fileWriter.drawing = true;
				iss >> word;
				if (iss) {
					fileWriter.fileName = word;
				} else {
					fileWriter.fileName = "out.png";
				}
			} 
			else if (word == "translate") { //translate x y z
				for(int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						translation[i] = atof(word.c_str());
					} else Error("Not enough arguments to translation.");
				}				
			} 
			else if (word == "rotate") { //rotate theta vec
				vec4 stuff(0,0,0,0);
				for(int i=0; i<4; i++) {
					iss >> word;
					if (iss) {
						stuff[i] = atof(word.c_str());
					} else Error("Not enough arguments to rotate.");
				}
				rotationAmount = stuff[0];
			 	rotateVec = vec3(stuff[1], stuff[2], stuff[3]);
			} 
			else if (word == "scale") { //scale x y z
				for(int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						scale[i] = atof(word.c_str());
					} else Error("Not enough arguments to scale.");
				}
			} 
			else if (word == "pl") { //pointlight x y z r g b
				vec4 pos;
				vec3 color;
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						pos[i] = atof(word.c_str());
					} else Error("Not enough arguments to PointLight");
				}
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						color[i] = atof(word.c_str());
					} else Error("Not enough arguments to PointLight");
				}
				pos[3] = 1;
				PLight* p = new PLight(pos, color);
				plights.push_back(p);
				if (DEBUG) cout << "Added point light to scene." << endl;
			} 
			else if (word == "dl") { //directionalight x y z r g 
				vec4 dir;
				vec3 color;
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						dir[i] = atof(word.c_str());
					} else Error("Not enough arguments to Directional Light");
				}
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						color[i] = atof(word.c_str());
					} else Error("Not enough arguments to Directional Light");
				}
				dir[3] = 0;
				DLight* d = new DLight(dir, color);
				dlights.push_back(d);
				if (DEBUG) cout << "Added directional light to scene." << endl;
			} 
			else if (word == "cleartrans"){ 
				translation = vec3(0,0,0);
				scale = vec3(1,1,1);
				rotateVec = vec3(0,0,0);
				rotationAmount = 0;
			} 
			else{
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
	
	viewport = Viewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	processArgs(argc, argv);
  	glutInit(&argc, argv);
	
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  	//The size and position of the window
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(-1, -1);
  	glutCreateWindow("Ray Tracer");
	
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