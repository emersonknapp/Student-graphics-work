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
#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 100
#define FRAMERATE 10
#define EPSILON 0.15
#define DEBUG true
#define BITSPERPIXEL 24

#define MAXRECURSION 2
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
mat4				identity;
Material			parsedMaterial;



//****************************************************
// Helper Functions
//****************************************************

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
vec3 shade(Ray ray, vec4 hitPoint, vec3 normal, int recursionDepth, int index) {

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
		float t;
		Material material;
		for (int k = 0; k < renderables.size() ; k++ ) {
			t = 1.0f;
			vec3 throwAway = vec3(0,0,0);
			renderables[k]->ray_intersect(lightCheck,t,throwAway);
		}
		vec4 intersection = ray.pos + t * ray.dir;
		if (t == 1.0f) { // then shade. if t != 1, then the light is blocked by another object
			material = renderables[index]->material;
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
	
	return 	color;
}


//***************************************************
// Function what actually draws to screen
//***************************************************
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	glBegin(GL_POINTS);


	Ray q = camera.generate_ray(-1,1);
	
	for (float i = 0; i < 1; i += 1.0/viewport.w) {
		for (float j = 0; j < 1; j+= 1.0/viewport.h) {
			Ray r = camera.generate_ray(i,j);
			
			float t = INT_MAX;
			float hit;
			int renderableIndex;
			bool hasHit = false;
			vec3 normal;
			
			for (int k = 0; k < renderables.size() ; k++ ) {
				//cout << "Testing against renderable " << k << endl;
				if(renderables[k]->ray_intersect(r,hit,normal)) {
					//cout << "Hit renderable " << k << endl;
					hasHit=true;
					if (hit < t) {
						t = hit;
						renderableIndex = k;
						setPixel(i,j, 1,1,1);
					}
				}
			}
			if (hasHit) {
				/*
				vec4 intersection = r.pos + t * r.dir; // at this point, t is minimum
				vec3 color = shade(r, intersection, normal, 1,k); // recursionDepth = 1 for debug purposes
	//			if (color != vec3(0,0,0)) cout << color << " at (" << i*viewport.w << "," << j*viewport.h << ")" << endl;
				color = vec3(1,1,1);
				setPixel(i*viewport.w, j*viewport.h, color[0], color[1], color[2]);
				use = false;
				*/
			}
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
//	exit(0);
}

//Reshape the viewport if the window is resized
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	
	glViewport(0,0, w, h);// sets the rectangle that will be the window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();				// loading the identity matrix for the screen
	
	gluOrtho2D(0, 1, 0, 1);

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
				parseMaterial.ka = vec3(v[0],v[1],v[2]);
				if (DEBUG) cout << "added ka = " << parseMaterial.ka << endl;
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
				iss >> word;
				if (iss) {
					parseMaterial.kr = atof(word.c_str());
					if (DEBUG) cout << "added kr = " << parseMaterial.kr << endl;
				} else {
					Error("Bad kr");
				}
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
				int r;
				iss >> word;
				if (iss) {
					r = atof(word.c_str());
					Sphere* sph = new Sphere(r);
					sph->translate(translation);
					sph->scale(scale);
					sph->rotate(rotationAmount, rotateVec);
					sph->material = parseMaterial;
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
				tri->translate(translation);
				tri->scale(scale);
				tri->rotate(rotationAmount, rotateVec);
				tri->material = parseMaterial;
				renderables.push_back(tri);
				if (DEBUG) cout << "Added triangle to scene." << endl;
			} else if (word == "camera") { //camera
				camera = Camera();
				camera.translate(translation);
				camera.scale(scale);
				camera.rotate(rotationAmount, rotateVec);
			} else if (word == "print") { //print outputfile
				fileWriter.drawing = true;
				iss >> word;
				if (iss) {
					fileWriter.fileName = word;
				} else {
					fileWriter.fileName = "out.png";
				}
			} else if (word == "translate") { //translate x y z
				vec3 stuff(0,0,0);
				for(int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						stuff[i] = atoi(word.c_str());
					} else Error("Not enough arguments to translation.");
				}
				translation = stuff;
			} else if (word == "rotate") { //rotate theta vec
				vec4 stuff(0,0,0,0);
				for(int i=0; i<4; i++) {
					iss >> word;
					if (iss) {
						stuff[i] = atoi(word.c_str());
					} else Error("Not enough arguments to rotate.");
				}
				rotationAmount = stuff[0];
			 	rotateVec = vec3(stuff[1], stuff[2], stuff[3]);
			} else if (word == "scale") { //scale x y z
				vec3 stuff(0,0,0);
				for(int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						stuff[i] = atof(word.c_str());
					} else Error("Not enough arguments to scale.");
				}
			 	scale = stuff;
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
			} else if (word == "dl") { //directionalight x y z r g 
				vec4 dir;
				vec3 color;
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						dir[i] = atoi(word.c_str());
					} else Error("Not enough arguments to Directional Light");
				}
				for (int i=0; i<3; i++) {
					iss >> word;
					if (iss) {
						color[i] = atoi(word.c_str());
					} else Error("Not enough arguments to Directional Light");
				}
				dir[3] = 0;
				DLight* d = new DLight(dir, color);
				dlights.push_back(d);
				if (DEBUG) cout << "Added directional light to scene." << endl;
			} else if (word == "cleartrans"){ 
				translation = vec3(0,0,0);
				scale = vec3(1,1,1);
				rotateVec = vec3(0,0,0);
				rotationAmount = 0;
			} else{
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
	identity = mat4(vec4(1,0,0,0),
					vec4(0,1,0,0),
					vec4(0,0,1,0),
					vec4(0,0,0,1));
	
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








