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
	vec3 t = scene->translation;
	glTranslatef(t[0] ,t[1], t[2]);
	vec3 r = scene->rotation;
	glRotatef(r[0],1,0,0);
	glRotatef(r[1],0,1,0);
	glRotatef(r[2],0,0,1);
	
	for (int i=0; i<scene->quadmeshes.size(); i++) {
		QuadMesh* mesh = scene->quadmeshes[i];

		glColor3f(.1,.1,0);
		glVertexPointer(3, GL_FLOAT, 0, mesh->verts);
		glNormalPointer(GL_FLOAT, 0, mesh->norms);
		
		if (scene->wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		if (scene->adaptiveSub) {
			glDrawElements(GL_TRIANGLES, 3*mesh->n_poly, GL_UNSIGNED_INT, mesh->indices);
		} else {
			glDrawElements(GL_QUADS, 4*mesh->n_poly, GL_UNSIGNED_INT, mesh->indices);
		}
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

void myIdleFunc() {
	float dt;
	// Compute the time elapsed since the last time the scence is redrawn
#ifdef _WIN32
	DWORD currentTime = GetTickCount();
	dt = (float)(currentTime - lastTime)*0.001f; 
#else
	timeval currentTime;
	gettimeofday(&currentTime, NULL);
	dt = (float)((currentTime.tv_sec - lastTime.tv_sec) + 1e-6*(currentTime.tv_usec - lastTime.tv_usec));
#endif
	scene->update(dt);

	// Store the time
	lastTime = currentTime;
	
	glutPostRedisplay();
}

//Deals with normal keydowns
void processNormalKeys(unsigned char key, int x, int y) {
	//escape, q, spacebar quit
	string name;
	switch(key) {
		case 27:
		case 'q':
		case 32:
			quitProgram();
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			name = "pic";
			name += key;
			name += ".png";
			imageWriter.fileName = name;
			imageWriter.init(viewport.w, viewport.h);
			break;
		case 's':
			if(scene->smoothShading) {
				glShadeModel(GL_FLAT);
			} else {
				glShadeModel(GL_SMOOTH);
			}
			scene->smoothShading = !scene->smoothShading;
			break;
		case 'w':
			scene->wireframe = !scene->wireframe;
			break;
		case '-':
		case '_':
			scene->translating[2] = -TRANSPEED;
			break;
		case '=':
		case '+':
			scene->translating[2] = TRANSPEED;
			break;
	}
}

//Deals with normal keyups
void processNormalKeyups(unsigned char key, int x, int y) {
	switch(key) {
		case '-':
		case '_':
		case '=':
		case '+':
			scene->translating[2] = 0;
			break;
	}
}

void processSpecialKeys(int key, int x, int y) {
	if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) {
		switch(key) {
			//Up, down are around x axis
			//Left, right around z
			case GLUT_KEY_UP:
				scene->translating[1] = TRANSPEED;
				break;
			case GLUT_KEY_DOWN:
				scene->translating[1] = -TRANSPEED;
				break;
			case GLUT_KEY_LEFT:
				scene->translating[0] = -TRANSPEED;
				break;
			case GLUT_KEY_RIGHT:
				scene->translating[0] = TRANSPEED;
				break;
		}
	}
	else {
		switch(key) {
			//Up, down are around x axis
			//Left, right around z
			case GLUT_KEY_UP:
				scene->rotating[0] = -ROTSPEED;
				break;
			case GLUT_KEY_DOWN:
				scene->rotating[0] = ROTSPEED;
				break;
			case GLUT_KEY_LEFT:
				scene->rotating[2] = ROTSPEED;
				break;
			case GLUT_KEY_RIGHT:
				scene->rotating[2] = -ROTSPEED;
				break;
		}
	}
}

void processSpecialKeyups(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			scene->translating[1] = 0;
			scene->rotating[0] = 0;
			break;
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			scene->translating[0] = 0;
			scene->rotating[2] = 0;
			break;
	}
	
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
	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glDepthFunc(GL_LEQUAL);
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
   	glEnable(GL_COLOR_MATERIAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	GLfloat LightAmbient[]= {0, 0, 0, 1};
	GLfloat LightDiffuse[]= {.4, .4, .4, 1};				
	GLfloat LightPosition[]= { 2, 5, 0, 1};
	GLfloat LightSpecular[]={.1,.1,.1,1};
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
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
		glutSpecialFunc(processSpecialKeys);
		glutSpecialUpFunc(processSpecialKeyups);
		glutIdleFunc(myIdleFunc);
	  	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	  	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	
		glutMainLoop();							// infinite loop that will keep drawing and resizing and whatever else
	}
  
  	return 0;
}