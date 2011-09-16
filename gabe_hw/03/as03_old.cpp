// Setup stuff 
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>


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

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265

using namespace std;

//
// Class Definitions
//

class Viewport {
public:
	int w, h; // width and height
};

class Circle {
public:
	int x,y; // position
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int sp; //power for specular
	
};

class PointLight {
public:
	vec3 position;
	vec3 color;
};

class DirectionalLight {
public:
	vec3 color;
};

//
// Global Vars
//
Viewport	viewport;

vector<PointLight> pointlights;

//DirectionalLights vector<DirectionalLight>;

Circle		circle1;

PointLight	pl1;


//
// Setup the Window
//
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent

	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,viewport.w, 0, viewport.h);
	
}



//
// Set Pixel With Colors
//
void setPixel(int x, int y, vec3 color) {
	glColor3f(color[0],color[1],color[2]);
	glVertex2f(x,y);
}

vec3 multAcross(vec3 a, vec3 b) {
	vec3 c;
	c[0] = a[0]*b[0];
	c[1] = a[1]*b[1];
	c[2] = a[2]*b[2];
	return c;
}

//
// Draw the circle
// 
void makeCircle(int x, int y, int radius){
	circle1.x = viewport.w/2;
	circle1.y = viewport.h/2;
	
	glBegin(GL_POINTS);

		for (int xDir = -radius; xDir <= radius; xDir++) {
			int yDist = (int)(sqrt((float)(radius*radius-xDir*xDir))+0.5f);
			for (int yDir = -yDist; yDir <= yDist; yDir++ ) {
				int zDist = (int)(sqrt((float)(radius*radius-yDir*yDir-xDir*xDir))+0.5f );
				if (zDist > 0) {
					for (int zDir = 0; zDir <= zDist; zDir++ ) {
						vec3 sphereNormal = vec3(xDir,yDir,zDir);
						sphereNormal.normalize();
						vec3 viewer = vec3(0.0f,0.0f,1.0f);
						vec3 color = vec3(0,0,0);
						
						for (int i = 0; i<pointlights.size(); i++) {
							PointLight pl = pointlights[i];
						
							vec3 lightNormal = pl.position*radius - sphereNormal;
							lightNormal.normalize();
						
							float diffuseDP = max(sphereNormal * lightNormal,0.0);
						
							vec3 reflect = -lightNormal+2*(lightNormal*sphereNormal)*sphereNormal;
						
							float reflectDP = pow(max(reflect*viewer,0.0),circle1.sp);
						
							vec3 specular = multAcross(circle1.specular,pl.color)*reflectDP;
						
							vec3 diffuse = multAcross(circle1.diffuse,pl.color)*diffuseDP;
											
							vec3 ambient = circle1.ambient;
							
							diffuse = vec3();
							ambient = vec3();

							color += diffuse+ambient+specular;
						
						}
						setPixel(x+xDir,y+yDir,color);
					}
				}
			}
		}
	glEnd();
	
}


void myFrameMove() {
	
#ifdef _WIN32
	Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
#endif

	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, viewport.w, 0, viewport.h);

	

}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer
	
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"


	// Start drawing
	makeCircle(circle1.x,circle1.y, min(viewport.w, viewport.h) / 2);

	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

//
// Main Loop
//
int main(int argc, char *argv[]) {
  	//This initializes glut
	
  	glutInit(&argc, argv);
	for (int i = 1; i < argc; i++ ) {
		if (strcmp(argv[i],"-pl") == 0) {
			PointLight pl;
			pl.position = vec3(atof(argv[i+1]),atof(argv[i+2]),atof(argv[i+3]));
			pl.color = vec3(atof(argv[i+4]),atof(argv[i+5]),atof(argv[i+6]));
			pointlights.push_back(pl);
			i+=6;
		}
		else if (strcmp(argv[i],"-ka") ==0 ) {
			circle1.ambient = vec3(atof(argv[i+1]),atof(argv[i+2]),atof(argv[i+3]));
			i+=3;
		}
		else if (strcmp(argv[i],"-kd") ==0 ) {
			circle1.diffuse = vec3(atof(argv[i+1]),atof(argv[i+2]),atof(argv[i+3]));
			i+=3;
		}
		else if (strcmp(argv[i],"-ks") == 0 ) {
			circle1.specular = vec3(atof(argv[i+1]),atof(argv[i+2]),atof(argv[i+3]));
			i+=3;
		}
		else if (strcmp(argv[i],"-sp") ==0) {
			circle1.sp = atof(argv[i+1]);
			i+=1;
		}
		
	}

  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  	// Initalize theviewport size
	viewport.w = 400;
  	viewport.h = 400;
	
  	//The size and position of the window
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(0, 0);
  	glutCreateWindow(argv[0]);
	
  	initScene();							// quick function to set up scene
  	glutDisplayFunc(display);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);					// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task

  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
	
  	return 0;
}
