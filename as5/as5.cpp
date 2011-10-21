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

void quitProgram() {
	//Make sure to delete stuff that was created using new.
	delete scene;
	FreeImage_DeInitialise();
	exit(0);
}

void quitProgram(int code) {
	delete scene;
	FreeImage_DeInitialise();
	exit(code);
}

void Error(string msg) {
	cout << msg << endl;
	quitProgram(1);
}

void Usage() {
	cout << "Bezier Subdivider: " << endl;
	cout << "./as5 filename param [-a]" << endl;
	cout << "	filename: a bezier patch file" << endl;
	cout << "	param: for uniform subdivision, the level of division. for adaptive, the acceptable error." << endl;
	cout << "	-a: specifies adaptive subdivision" << endl;
	quitProgram(1);
}

//***************************************************
// Function what actually draws to screen
//***************************************************
void myDisplay() {
	if (!imageWriter.drawing) {
		glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	}
	
	glBegin(GL_QUADS);
	
	glColor3f(1, 1, 1);
	glVertex3f(-1, 1, -10);
	glVertex3f(1, 1, -10);
	glVertex3f(1, -1, -10);
	glVertex3f(-1, -1, -10);
	
	glEnd();
	
	
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45, w/h, 0.1f, 1000.0f); //TODO: how does Ortho3D work?

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
	} else if (key=='s') {
		if(scene->smoothShading) {
			glShadeModel(GL_FLAT);
		} else {
			glShadeModel(GL_SMOOTH);
		}
		scene->smoothShading = !scene->smoothShading;
	} else if (key=='w') {
		scene->wireframe = !scene->wireframe;
	}
}

//Deals with normal keyups
void processNormalKeyups(unsigned char key, int x, int y) {

}

//****************************************************
// sets the window up
//****************************************************
void initScene(){
	//Get all the OpenGL stuff up and running. 
	//We'll need to have some nice definitions for lights values
	//Possible just modify existing light classes
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	//glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	//glEnable(GL_LIGHT1);
	myReshape(viewport.w,viewport.h);
}


void processArgs(int argc, char* argv[]) {
	if (argc < 3) {
		Usage();
	}
	int i=1;
	string arg = argv[i++];
	scene = new Scene(arg);
	if (DEBUG) cout << "Parsing scene " << arg << endl;
	arg = argv[i++];
	scene->param = atof(arg.c_str());
	if (DEBUG) cout << "Parameter " << atof(arg.c_str()) << endl;
	
	if (argc >= 4) {
		for ( ; i<argc; i++) {
			string arg = argv[i];
		
			if (arg.compare("-a") == 0) {
				scene->adaptiveSub = true;
				if (DEBUG) cout << "Using adaptive subdivision." << endl;
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

	
}


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	
	srand((unsigned)time(NULL));
	
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
	  	glutCreateWindow("Bezier Subdivision");
	
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