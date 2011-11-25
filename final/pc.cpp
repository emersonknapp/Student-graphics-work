#include "pc.h"

using namespace std;

//****************************************************
// Global Variables
//****************************************************
Viewport			viewport;
Scene*				scene;
ImageWriter*		imageWriter;


//****************************************************
// Helper Functions
//****************************************************

vec3 traceRay(Ray, int);

void setPixel(float x, float y, float r, float g, float b) {
	unsigned char color[3];
	color[0] = min((int)floor(r*255), 255);
	color[1] = min((int)floor(g*255), 255);
	color[2] = min((int)floor(b*255), 255);
	imageWriter->setPixel(x, y, color);
}

void quitProgram(int exitCode) {
	//Make sure to delete stuff that was created using new.
	delete imageWriter;
	delete scene;
	FreeImage_DeInitialise();
	exit(exitCode);
}

void Error(string msg) {
	cerr << msg << endl;
	quitProgram(1);
}

void Warning(string msg) {
	cerr << "Warning: " << msg << endl;
}

void Usage() {
	cout << "Photon Cannon v" << VERSION_NUMBER << endl
	<< "Usage:" << endl
	<< "    pc [-s scene] [-pr outputfile] [-px x y]" << endl
	<< "        -s: scene file" << endl
	<< "        -pr: output print file name, defaults to \"out.png\"" << endl
	<< "        -px: the size of the output image, in pixels" << endl
	;
	quitProgram(0);
}


//***************************************************
// Do phong shading on a point
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
	for (vector<Light*>::iterator it = scene->lights.begin(); it != scene->lights.end(); ++it) {
		Light* currentLight = *it;
		
		Material material;
		bool shadePixel = true;
		float t;

		
		//shadow ray
		Ray lightCheck = Ray(hitPoint+EPSILON*norm, currentLight->lightVector(hitPoint));
		//Linearly search renderables for shadow intersection
		int j = 0;
		for (vector<Renderable*>::iterator it = scene->renderables.begin(); it != scene->renderables.end(); ++it) {
			Renderable* rend = *it;
			if((t=rend->ray_intersect(lightCheck)) <= lightCheck.dir.length() && t>0 && index != j) {	
				shadePixel = false;
				break;
			}
			++j;
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
		// interesting... if we hit the rend->material.ri > 0 case, then we don't have a refracted ray?
		// we get this black ring around the sphere in example test4.t, and I think it's because the rays with
		// r.curRI = 1.33 don't hit anything in the scene for some reason
		if (rend->material.ri > 0) {

			float c1 = (n*d);
			float nn;
			float curRI, oldRI;
			//TODO: make it so that when we refract the ray, we set oldRI to the cur.curRI and *then* update the cur.curRI
			if (c1 < 0) { // ray hits outside of object, so we set ray.ri to the object's ri
				nn = rend->material.ri / r.curRI;
				oldRI = r.curRI;
				curRI = rend->material.ri;
				n=-normal.dehomogenize().normalize();
			} else { // ray hits inside of object, then we know we're going to what we had before (oldRI)
				// we want to set rend->material.ri to the *old* r.curRI (before it hit the object), but for now, jsut set to 1.0
				nn = r.curRI / 1.0;
				curRI = oldRI;
				oldRI = 1.0;
				n=normal.dehomogenize().normalize();
			}
			float c2 = 1.0-(pow(nn,2) * (1.0 - pow(c1,2)));
			if (c2 >= 0.0) {
				vec3 tmp1 = (nn*d);
				vec3 tmp2 = (nn*c1-sqrt(c2))*(n);
				vec3 tmp3 = tmp1 + tmp2;
				tmp3.normalize();
				vec4 rayDirection = vec4(tmp3,0);
				Ray refractedRay = Ray(hitPoint+EPSILON*rayDirection,rayDirection,curRI,oldRI,true);
				vec3 refractedColor = traceRay(refractedRay, depth+1);
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
void render() {
	
	/*Logging output */
	int onepercent = viewport.w/100;
	int progress = 0.0;
	int nextpercent = onepercent;
	/*End*/
	
	for (float x = 0; x < viewport.w; x++) {
		for (float y = 0; y < viewport.h; y++) {			
			Camera* camera = scene->camera;			
			Ray camRay = camera->generate_ray(x/viewport.w,y/viewport.h);	
			//cout << camRay.dir << " " << camRay.pos << endl;		
			//vec4 color = camRay.dir;
			vec3 color = traceRay(camRay, 0);
			
			//TODO: this next line adds ambient color to *every* pixel, regardless of whether or not it's on an object
			color += scene->ambience;
			setPixel(x,y,color[0], color[1], color[2]);			
		}
		
		/* logging output */
		progress += 1.0;
		if (progress >= nextpercent) {	
			//clog << ".";
			clog << "Rendering: " << nextpercent * 100 / viewport.w << "%" << "\r";
			nextpercent += onepercent;
		}
		/* End */
	}
	
	cout << endl << "Completed render! Outputting to file." << endl;
	imageWriter->printScreen();
	
}

void processArgs(int argc, char* argv[]) {
	if (argc < 2) {
		Usage();
	}
	
	string arg;
	for (int i=1; i<argc; i++) {
		
		arg = argv[i];
		
		if (arg.compare("-s") == 0) {
			scene->parseScene(argv[++i]);
		} else if (arg.compare("-pr")==0) { //TODO: fix this.
			imageWriter->fileName = argv[++i];
		} else if (arg.compare("-px")==0) {
			int width = atoi(argv[++i]);
			int height = atoi(argv[++i]);
			viewport.w = width;
			viewport.h = height;
			imageWriter->setSize(width, height);
		}
	}
	
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	
	srand((unsigned)time(NULL));
	
	//TODO: init stuff
	viewport = Viewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	imageWriter = new ImageWriter(viewport.w, viewport.h);
	scene = new Scene();
	
	processArgs(argc, argv);
	
	render();

  	return 0;
}
