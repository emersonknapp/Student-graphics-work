// Simple OpenGL example for CS184 sp08 by Trevor Standley, modified from sample code for CS184 on Sp06
#include <cstdlib>
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

#ifdef _WIN32
static DWORD lastTime;
#else
static struct timeval lastTime;
#endif

#define PI 3.14159265
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define STAR_SPEED .01
#define NUM_STARS 100
#define STAR_INITIAL_SIZE .01
#define FRAMERATE 60

using namespace std;
bool turbo = false;


//****************************************************
// Some Classes
//****************************************************

double randscreen (void) {
	return (rand() / double(RAND_MAX) * 2) - 1;
}

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
	void setWidth (int width) {
		w = width;
	}
	void setHeight (int height) {
		h = height;
	}
	int getWidth () {
		return w;
	}
	int getHeight () {
		return h;
	}
private:
	int w;
	int h;
};

class Star {
public:
	Star () {
		x = randscreen();
		y = randscreen();
		size = STAR_INITIAL_SIZE;
	}
	void update () {
		double radius = sqrt(x*x + y*y);
		double angle = atan(y/x);
		
		if (x < 0) {
			angle += PI;
		}
		
		double speed = STAR_SPEED;
		if (turbo) {
			speed *= 3;
		}
		radius += speed;


		x = radius * cos(angle);
		y = radius * sin(angle);
		
		size += .0005;

		if (x > 1 || x < -1 || y > 1 || y < -1) {
			x = randscreen();
			y = randscreen();
			size = STAR_INITIAL_SIZE;
		}
	}
	void draw () {
		glColor3f(1.0, 1.0, 1.0);
		
		glBegin(GL_POLYGON);
			glVertex3f(x, y, 0.0f);
			glVertex3f(x+size, y, 0.0f);
			glVertex3f(x+size, y+size, 0.0f);
			glVertex3f(x, y+size, 0.0f);
		glEnd();
	}
private:
	float x;
	float y;
	float size;
};

//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
vector<Star> stars;


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.setWidth(w);
	viewport.setHeight(h);

	glViewport(0,0,viewport.getWidth(),viewport.getHeight());// sets the rectangle that will be the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//------------------------------------------------------------
}

//******************************
// deals with normal keypresses
//******************************
void processNormalKeys(unsigned char key, int x, int y) {
	//escape, q quit
	if (key == 27 || key == 'q') {
		exit(0);
	}
	if (key == 32) {
		turbo = true;
	}
}

void processNormalKeyups(unsigned char key, int x, int y) {
	if (key == 32) {
		turbo = false;
	}
}

//****************************************************
// sets the window up
//****************************************************
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	
	myReshape(viewport.getWidth(),viewport.getHeight());
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {
	cout << "frame" << endl;
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"
	
	
	for (int i=0; i<stars.size(); i++) {
		stars[i].update();
		stars[i].draw();
	}
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



void myTick( int t ) {
	glutPostRedisplay();
	// Reset timer
	glutTimerFunc(t, myTick, t);
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	srand((unsigned)time(NULL));
	int fr = 1000/FRAMERATE;
  	//This initializes glut
  	glutInit(&argc, argv);
  
  	//This tells glut to use a double-buffered window with red, green, and blue channels 
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	viewport.setWidth(SCREEN_WIDTH);
	viewport.setHeight(SCREEN_HEIGHT);
	
	for (int i=0; i<NUM_STARS; i++) {
		stars.push_back(Star());
	}
	
  	//The size and position of the window
  	glutInitWindowSize(viewport.getWidth(), viewport.getHeight());
  	glutInitWindowPosition(-1, -1);
  	glutCreateWindow("SPACE!");

  	initScene();							// quick function to set up scene
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(processNormalKeyups);
  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	
	glutTimerFunc(fr, myTick, fr);
	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
  	return 0;
}








