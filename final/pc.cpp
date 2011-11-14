#include "pc.h"

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

vec3 traceRay(Ray, int);

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
	
	//Loop through lights
	for (int i=0; i<scene->lights.size(); i++) {
		Light* currentLight = scene->lights[i];
		
		float t = T_MAX;
		Material material;
		bool shadePixel = true;
		float newT;

		
		//shadow ray
		Ray lightCheck = Ray(hitPoint+EPSILON*norm, currentLight->lightVector(hitPoint));
		//Linearly search renderables for shadow intersection
		for (int j = 0; j < scene->renderables.size(); j++ ) {
			Renderable* rend = scene->renderables[j];
			if((newT=rend->ray_intersect(lightCheck)) <= lightCheck.dir.length() && newT>0 && index != j) {	
				shadePixel = false;
				break;
			}
		}
		
		if (shadePixel) {
			material = scene->renderables[index]->material;
			vec3 lightColor = currentLight->intensity;
			vec3 lightVector = currentLight->lightVector(hitPoint+EPSILON*norm).dehomogenize();
			lightVector.normalize();
			vec3 viewVector = (r.pos-hitPoint).dehomogenize();
			viewVector.normalize();
			vec3 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
			reflectionVector.normalize();
			
			//Diffuse term
			color += prod(material.kd, lightColor)*max((lightVector*normal), 0.0);
			//Specular term
			vec3 specular = prod(material.ks, lightColor)*pow(max(reflectionVector*viewVector, 0.0), material.sp);
			// if the material is refractive, we only use specular ???
//			if (material.ri != 0) color = specular; else color += specular;
			color += specular;
		}
		
	}
	return color;
}


//Traces a ray out, collecting outputs from shading, reflections, etc.
vec3 traceRay(Ray r, int depth) {
	//Recursion cutoff check
	if (depth > MAXRECURSION) {
		return vec3(0,0,0);
	}
	
	int renderableIndex=-1;
	float t = T_MAX;
	bool hasHit = false;
	vec3 color = vec3(0,0,0);
	
	hasHit = scene->rayIntersect(r, t, renderableIndex);

	if (hasHit) {
		Renderable* rend = scene->renderables[renderableIndex];
		vec4 hitPoint = r.pos + t*r.dir;
		vec4 normal = rend->normal(hitPoint);

		color += shade(r, hitPoint, normal, renderableIndex);
		
		vec3 n = -normal.dehomogenize();
		vec3 d = r.dir.dehomogenize();
		n.normalize();
		d.normalize();
		
		vec3 refl = d - 2*(d*n)*n;
		refl.normalize();
		// to note: air has ri = 1.0 (should be default)
		// non refractive materials have ri = 0
		// all else are ri != 1.0 or != 0
		// if hasHit, check the refractive index (ri) of the material we hit.
		// if ri > 0, then the material is refractive, so we calculate the angle of refraction, and send out a new ray
		if (rend->material.ri > 0) {
			float c1 = -(n*d);
			float refractiveIndex;
			// if current index of ray is the same as the one we're hitting, it means we're inside something
			// and going outside, so we set the refractiveIndex to 1.0 (air)
			if (r.ri == rend->material.ri) refractiveIndex = 0.0f; else refractiveIndex = rend->material.ri;
//			float nn = r.ri / refractiveIndex;
			// get dotproduct of normal and incident ray (as unit vector). If absolute value is less than 90, we're outside, if > 90, we're inside
			
			float nn = refractiveIndex / r.ri;
			float c2 = 1-pow(nn,2) * (1 - pow(c1,2));
			if (c2 > 0.0) {
				vec3 tmp1 = (nn*d);
				vec3 tmp2 = (nn*c1-sqrt(c2))*normal.normalize();
				vec3 tmp3 = tmp1 + tmp2;
				vec3 rayDirection = tmp3.normalize();
				Ray refractedRay = Ray(hitPoint,rayDirection,rend->material.ri);
				vec3 refractedColor = traceRay(refractedRay, depth+1);
				//TODO: is there some constant we multiply this by?
//				r.ri = rend->material.ri;
				color += refractedColor;
			}
		}
		//calculate reflections
		Ray newray = Ray(hitPoint+EPSILON*normal, vec4(refl,0));
		vec3 kr = rend->material.kr;
		vec3 reflColor = traceRay(newray, depth+1);
		color += prod(kr,reflColor);
		return color;
	} else { 
		return vec3(0,0,0);
	}
}
//***************************************************
// Function what actually draws to screen
//***************************************************
void myDisplay() {
	if (!imageWriter.drawing) {
		glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer (sets everything to black)
		glBegin(GL_POINTS);
	}
	
	for (float x = 0; x < viewport.w; x++) {
		for (float y = 0; y < viewport.h; y++) {
			Camera* camera = scene->camera;			
			Ray camRay = camera->generate_ray(x/viewport.w,y/viewport.h);	
			//cout << camRay.dir << " " << camRay.pos << endl;		
			//vec4 color = camRay.dir;
			vec3 color = traceRay(camRay, 0);
			color += scene->ambience;
			setPixel(x,y,color[0], color[1], color[2]);
		}	
	}
	
	if(!imageWriter.drawing) {
		glEnd();
		
		glFlush();
		glutSwapBuffers();					// swap buffers (we earlier set double buffer)
	}
	
	
	
	if (imageWriter.drawing) {
		
		if (DEBUG) cout << "Completed render! Outputting to file." << endl;
		imageWriter.printScreen();
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
	  	glutCreateWindow("Final Project Bitchessssss");
	
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
