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
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);				// clear the color buffer (sets everything to black)
	
	glLoadIdentity();
	
	glTranslatef(0,0,0);
	for (int i=0; i<scene->quadmeshes.size(); i++) {
		QuadMesh* mesh = scene->quadmeshes[i];
		
		glBegin(GL_POINTS);
		glColor3f(1,1,1);
		for (int j=0; j<mesh->n_poly; j++) {
			vec4 q;
			vec3 n;
			for (int k=0; k<4; k++) {
				
				int q = mesh->indices[j*4+k];
				//vec3 n = vec3(mesh->norms[q*4], mesh->norms[q*4+1], mesh->norms[q*4+2]);
				//vec3 v = vec3(mesh->verts[q*4], mesh->verts[q*4+1], mesh->verts[q*4+2]);
				vec3 n = mesh->normsVec[q];
				vec3 v = mesh->vertsVec[q];
				//cout << n << v << endl;
				glNormal3f(n[0], n[1], n[2]);
				glVertex3f(v[0], v[1], v[2]);	
			}
			//cout << endl << endl;
		}
		//cout << "**********************************" << endl;
		glEnd();
		
		//glColor3f(1,1,1);
		//glVertexPointer(3, GL_FLOAT, 0, mesh->verts);
		//glNormalPointer(GL_FLOAT, 0, mesh->norms);
		//glDrawElements(GL_QUADS, 4*mesh->n_poly, GL_UNSIGNED_INT, mesh->indices);
		
	}
	
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)

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
	
	gluPerspective(45, w/h, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
	GLfloat LightAmbient[]= {1, 1, 1, 1};
	GLfloat LightDiffuse[]= {1, 1, 1, 1};				
	GLfloat LightPosition[]= { 0, 0, 0, 1};
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	myReshape(viewport.w,viewport.h);
}


void processArgs(int argc, char* argv[]) {
	if (argc < 3) {
		Usage();
	}
	int i=1;
	string filename = argv[i++];
	string p = argv[i++];
	if (DEBUG) cout << "Parsing scene " << filename << endl;
	if (DEBUG) cout << "Parameter: " << atof(p.c_str()) << endl;
	scene = new Scene(filename, atof(p.c_str()));
	
	string arg;
	if (argc >= 4) {
		for ( ; i<argc; i++) {
			arg = argv[i];
		
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
	
	  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

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