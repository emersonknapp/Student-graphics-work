#include "as5.h"

using namespace std;

//****************************************************
// Global Variables
//****************************************************
Viewport			viewport;
Scene*				scene;
ImageWriter			imageWriter;


//****************************************************
// Helper Functions
//****************************************************

void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
	if (imageWriter.drawing) {
		unsigned char color[3];
		color[0] = min((int)floor(r*255), 255);
		color[1] = min((int)floor(g*255), 255);
		color[2] = min((int)floor(b*255), 255);
		imageWriter.setPixel(x, y, color);
	} else {
		glColor3f(r, g, b);
		glVertex2f(x, y);
	}
}

void Error(string msg) {
	cout << msg << endl;
	exit(1);
}

void quitProgram() {
	//Make sure to delete stuff that was created using new.
	delete scene;
	FreeImage_DeInitialise();
	exit(0);
}

//***************************************************
// Function what actually draws to screen
//***************************************************
void myDisplay() {
	if (!imageWriter.drawing) {
		glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	}
	
	//DRAW!
	
	
	if(!imageWriter.drawing) {
		glFlush();
		glutSwapBuffers();					// swap buffers (we earlier set double buffer)
	}
	if (imageWriter.drawing) {
		imageWriter.printScreen();
		quitProgram();	
	}
}

//Reshape the viewport if the window is resized
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	
	glViewport(0,0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluOrtho2D(0, viewport.w, 0, viewport.h); //TODO: how does Ortho3D work?

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
		imageWriter.fileName = name;
		imageWriter.init(viewport.w, viewport.h);
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
		vec3 translation(0,0,0);
		vec3 scale(1,1,1);
		vec3 rotation(0,0,0);
		
		string arg = argv[i];
		
		if (arg.compare("-s") == 0) {
			scene = new Scene(argv[++i]);
		} else if (arg.compare("-pr")==0) {
			imageWriter.init(viewport.w, viewport.h);
			imageWriter.glOn = false;
			imageWriter.fileName = argv[++i];
		} else if (arg.compare("-px")==0) {
			int width = atoi(argv[++i]);
			int height = atoi(argv[++i]);
			viewport.w = width;
			viewport.h = height;
			imageWriter.setSize(width, height);
		}
	}

	
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	
	srand((unsigned)time(NULL));
	
	//Initialize FreeImage library
	
	//cout << "FreeImage " << FreeImage_GetVersion() << endl;
	//cout << FreeImage_GetCopyrightMessage() << endl;
	
	viewport = Viewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	processArgs(argc, argv);
	
	if (imageWriter.drawing) {
		myDisplay();
	} else {
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
	}
  
  	return 0;
}