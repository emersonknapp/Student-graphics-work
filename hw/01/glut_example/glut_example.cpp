// Simple OpenGL example for CS184 sp08 by Trevor Standley, modified from sample code for CS184 on Sp06
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

using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
	int w, h; // width and height
};



//****************************************************
// Global Variables
//****************************************************

Viewport	viewport;
float orig1X, orig1Y, orig2X, orig2Y;
int activeSquare = 0;


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;

	glViewport(0,0,viewport.w,viewport.h);// sets the rectangle that will be the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// loading the identity matrix for the screen

	//----------- setting the projection -------------------------
	// glOrtho sets left, right, bottom, top, zNear, zFar of the chord system


	// glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
	// glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center

	glOrtho(-1, 1, -1, 1, 1, -1);	// resize type = stretch

	//------------------------------------------------------------
}


//****************************************************
// sets the window up
//****************************************************
void initScene(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear to black, fully transparent
	
	myReshape(viewport.w,viewport.h);
}

//*************************
// function to change the shape based on the mouse position/clicks
//*********************


void changeFromMouse(int x, int y) {
	if (activeSquare == 0) {
		orig1X = (viewport.w/2 - (float)x)/100.0;
		orig1Y = (viewport.h/2 - (float)y)/100.0;
	}
	else if (activeSquare == 1) {
		orig2X = (viewport.w/2 - (float)x)/100.0;
		orig2Y = (viewport.h/2 - (float)y)/100.0;
	} 
}

void changeSquareFromClick(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (activeSquare == 1) {
			activeSquare = 0;
		} else {
			activeSquare = 1;
		}
	}
}


//***************************************************
// function that does the actual drawing
//***************************************************
void myDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"
	
	//----------------------- code to draw objects --------------------------

	glColor3f(1.0f,0.3f,0.0f);

	//square 1
	glBegin(GL_LINE_STRIP);
		glVertex3f(-0.3f-orig1X,0.3f+orig1Y,0.0f);
		glVertex3f(0.3f-orig1X,0.3f+orig1Y,0.0f);
		glVertex3f(0.3f-orig1X,-0.3f+orig1Y,0.0f);
		glVertex3f(-0.3f-orig1X,-0.3f+orig1Y,0.0f);
		glVertex3f(-0.3f-orig1X,0.3f+orig1Y,0.0f);
	glEnd();
	
	//square 2
	glBegin(GL_LINE_STRIP);
		glVertex3f(-0.3f-orig2X,0.3f+orig2Y,0.0f);
		glVertex3f(0.3f-orig2X,0.3f+orig2Y,0.0f);
		glVertex3f(0.3f-orig2X,-0.3f+orig2Y,0.0f);
		glVertex3f(-0.3f-orig2X,-0.3f+orig2Y,0.0f);
		glVertex3f(-0.3f-orig2X,0.3f+orig2Y,0.0f);
	glEnd();
	
	//connecting lines between vertices
	glBegin(GL_LINE_STRIP);
		glVertex3f(-0.3f-orig1X,0.3f+orig1Y,0.0f);
		glVertex3f(-0.3f-orig2X,0.3f+orig2Y,0.0f);
	glEnd();
		
	glBegin(GL_LINE_STRIP);
		glVertex3f(0.3f-orig1X,0.3f+orig1Y,0.0f);
		glVertex3f(0.3f-orig2X,0.3f+orig2Y,0.0f);
	glEnd();
	
	glBegin(GL_LINE_STRIP);
		glVertex3f(0.3f-orig1X,-0.3f+orig1Y,0.0f);
		glVertex3f(0.3f-orig2X,-0.3f+orig2Y,0.0f);
	glEnd();
	
	glBegin(GL_LINE_STRIP);
		glVertex3f(-0.3f-orig1X,-0.3f+orig1Y,0.0f);
		glVertex3f(-0.3f-orig2X,-0.3f+orig2Y,0.0f);
	glEnd();
	//-----------------------------------------------------------------------
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}


//****************************************************
// called by glut when there are no messages to handle
//****************************************************
void myFrameMove() {
	
#ifdef _WIN32
	Sleep(10);						//give ~10ms back to OS (so as not to waste the CPU)
#endif

	glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  	//This initializes glut

  	glutInit(&argc, argv);
  
  	//This tells glut to use a double-buffered window with red, green, and blue channels 
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  	// Initalize theviewport size
  	viewport.w = 400;
  	viewport.h = 400;

  	//The size and position of the window
  	glutInitWindowSize(viewport.w, viewport.h);
  	glutInitWindowPosition(0, 0);
  	glutCreateWindow("CS184!");
	

  	initScene();							// quick function to set up scene
	glutPassiveMotionFunc(changeFromMouse);
	glutMouseFunc(changeSquareFromClick);

  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something

  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
  	glutIdleFunc(myFrameMove);				// function to run when not handling any other task
  	glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
  
  	return 0;
}
