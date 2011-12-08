#include <omp.h>
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
//TODO: option where you can see where the photons landed
void Usage() {
	cout << "Photon Cannon v" << VERSION_NUMBER << endl
	<< "Usage:" << endl
	<< "    pc [-s scene] [-pr outputfile] [-px x y] [-a n]" << endl
	<< "        -s: scene file" << endl
	<< "        -pr: output print file name, defaults to \"out.png\"" << endl
	<< "        -px: the size of the output image, in pixels (default 800x800)" << endl
	<< "        -a: antialiasing, n-by-n rays per pixel, defaults to 1" << endl
	<< "		-j: uses jittery antialiasing (still requires -a)" << endl
	<< "		-ph: how many photons to shoot out (in thousands) (if not specified, defaults to FUCKYOU)" << endl
	<< "		-e: epsilon for photon gathering" << endl
	<< "		-c: number of caustic photons (in thousands) (defaults to 0)" << endl;
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
		float t = T_MAX;
		int renderableIndex=-1;

		//shadow ray
		Ray lightCheck = Ray(hitPoint+EPSILON*norm, currentLight->lightVector(hitPoint));
		shadePixel = !(scene->rayIntersect(lightCheck, t, renderableIndex));
		
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

vec3 diffuseRayColor(Ray r) {
	vec3 color = vec3(0,0,0);
	// intersect w/ objet, get the gatherEpsilon aabb box and average those photon colors

	int renderableIndex=-1;
	float t = T_MAX;
	bool hasHit = false;
	
	hasHit = scene->rayIntersect(r, t, renderableIndex);
	if (hasHit) {
		vec4 hitPoint = r.pos + t*r.dir;
		vec4 normal = scene->renderables[renderableIndex]->normal(hitPoint);
		vec3 mins = hitPoint.dehomogenize() - vec3(viewport.gatherEpsilon);
		vec3 maxes = hitPoint.dehomogenize() + vec3(viewport.gatherEpsilon);
		AABB gatherBox = AABB(mins,maxes);	
		vector<photIt> nearPhotons;
		if (scene->photonTree->gatherPhotons(&gatherBox,nearPhotons)) {
			for (size_t i=0; i< nearPhotons.size(); i++) {
				color += (*nearPhotons[i])->color;
			}
			color = (2.0/3.0) * color / (PI*pow(viewport.gatherEpsilon, 3.0f));
			
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
		//************************************
		//AMBIENT HACK FOR DIRECT ILLUMINATION
		if (depth==0 and !(viewport.photons)) {
			color += scene->ambience;
		}
			
		Renderable* rend = scene->renderables[renderableIndex];
		vec4 hitPoint = r.pos + t*r.dir;
		vec4 normal = rend->normal(hitPoint);

		// generating diffuse rays
		if (viewport.photons) {
			
			bool rawPhotons = true;
			if (rawPhotons) {
				Ray photCheck = Ray(r.pos, r.dir);
				color = diffuseRayColor(photCheck);
				return color;
			}
			return color;
			//****************
			//INDIRECT ILLUMINATION
			int gather_rays = 50;
			#pragma omp parallel for shared(color)
			for (int i = 0; i < gather_rays; i++) {
				vec3 point = randomHemispherePoint(normal);
				
				vec4 diffuseRayDirection = vec4(point,0);
				//generate diffuse ray
				Ray diffuseRay = Ray(hitPoint+EPSILON*normal, diffuseRayDirection);
				color += diffuseRayColor(diffuseRay) * max(0.0, diffuseRayDirection * normal) / (float)gather_rays;
			}
			return color;
		}
		
		//*************
		//SPECULAR ( and DIFFUSE if not PHOTON MAPPING )
		color += shade(r, hitPoint, normal, renderableIndex);
		
		vec3 n = -normal.dehomogenize();
		vec3 d = r.dir.dehomogenize();
		n.normalize();
		d.normalize();
		
		vec3 refl = d - 2*(d*n)*n;
		refl.normalize();
		
		// *******************************
		// COMPUTE REFRACTION
		
		float cosTheta = r.dir * normal;
		float riOld, riNew;
		vec4 refracted;
		vec3 norm = normal.dehomogenize();
		if (cosTheta < 0) { 
			//Ray hits outside of object
			riOld = 1.0;
			riNew = rend->material.ri;
		} else {
			//Ray hits inside of object
			riOld = rend->material.ri;
			riNew = 1.0;
		}
		float cos2Phi = 1 - ((pow(riOld, 2) * (1-pow(cosTheta, 2))) / pow(riNew, 2));
		if (cos2Phi > 0) {
			//not totally internally reflected
			refracted = (riOld * (r.dir - normal*cosTheta) / riNew) - (normal * sqrt(cos2Phi));
			Ray refractedRay = Ray(hitPoint + EPSILON*normal, refracted);
			color += traceRay(refractedRay, depth+1);
		} else; //Totally internally reflected
		
		
		/// *******************************
		// COMPUTE REFLECTION
		Ray reflRay = Ray(hitPoint+EPSILON*normal, vec4(refl,0));
		vec3 kr = rend->material.kr;
		vec3 reflColor = traceRay(reflRay, depth+1);
		color += prod(kr,reflColor);
		
		// *********************************
		// COMPUTE TEXTURE MAPPING
		if (rend->material.texture.exists) color += rend->textureColor(hitPoint);
		return color;
	} else { 
		return vec3(0,0,0);
	}
}


void tracePhoton(Photon* phot, int reflDepth) {
	if (reflDepth > PHOTCURSION) {
		return;
	}
	
	int renderableIndex=-1;
	float t = T_MAX;
	bool hasHit = false;

	hasHit = scene->rayIntersect(*phot, t, renderableIndex);

	if (hasHit) {
		vec4 hitPoint = phot->pos + t*phot->dir;
		Renderable* rend = scene->renderables[renderableIndex];
		Material mat = rend->material;
		vec3 kd = mat.kd;
		vec3 ks = mat.ks;
		
		float probReflect = max(max(kd[0]+ks[0], kd[1]+ks[1]), kd[2]+ks[2]);
		float randPick = rand01();
		
		if (randPick < probReflect) {
			vec4 normal = rend->normal(hitPoint);
			hitPoint = hitPoint + EPSILON*normal;
			hitPoint[3] = 1;			
			
			vec3 n = -normal.dehomogenize();
			vec3 d = phot->dir.dehomogenize();
			n.normalize();
			d.normalize();

			vec3 refl = d - 2*(d*n)*n;
			refl.normalize();
						
			double probDiffuseReflect = sum(kd) / (sum(kd)+sum(ks)) * probReflect;
			double probSpecularReflect = probReflect - probDiffuseReflect;
			phot->pos = hitPoint;
			if (randPick < probDiffuseReflect) {
				//Diffuse reflection
				vec3 newColor = prod(kd, phot->color)/probDiffuseReflect;
				scene->photons.push_back(phot);
				vec3 newDir = randomHemispherePoint(normal);
				Photon* reflPhoton = new Photon(phot->pos, newDir, newColor);

				tracePhoton(reflPhoton, reflDepth+1);
				
			} else {
				if (mat.ri > 0) {
					//Refraction
					float cosTheta = phot->dir * normal;
					float riOld, riNew;
					vec4 refracted;
					vec3 norm = normal.dehomogenize();
					if (cosTheta < 0) { 
						//Ray hits outside of object
						riOld = 1.0;
						riNew = mat.ri;
					} else {
						//Ray hits inside of object
						riOld = mat.ri;
						riNew = 1.0;
					}
					float cos2Phi = 1 - ((pow(riOld, 2) * (1-pow(cosTheta, 2))) / pow(riNew, 2));
					if (cos2Phi > 0) {
						//not totally internally reflected
						refracted = (riOld * (phot->dir - normal*cosTheta) / riNew) - (normal * sqrt(cos2Phi));
						phot->dir = refracted;
						phot->pos = hitPoint;
						tracePhoton(phot, reflDepth+1);
					} else; //Totally internally reflected
				} else {
					//Specular reflection
					phot->dir = vec4(refl, 0);
					phot->color = prod(ks, phot->color)/probSpecularReflect;
					tracePhoton(phot, reflDepth+1);
				}
			}
		} else; //Absorboloth. <-- I like that!
	}
}

void photonCannon() {
	vector<Photon*> photonCloud;
	
	for (vector<Light*>::iterator it = scene->lights.begin(); it != scene->lights.end(); ++it) {
		Light* currentLight = *it;
		currentLight->generatePhotons(photonCloud, viewport.photonsPerLight, scene->kdTree->aabb);
	}
	//iterate through photonCloud, push photons that intersect onto scene->photons
	for (photIt phot = photonCloud.begin(); phot != photonCloud.end(); ++phot) {
		tracePhoton(*phot, 0);
	}
	//store photons that hit a renderable into kdtree
	cout << photonCloud.size() << endl;
	cout << viewport.photonsPerLight << endl;
	cout << scene->photons.size() << endl;
	if (viewport.causticPhotonsPerLight == 0) scene->photonTree = new PhotonTree(scene->photons.begin(), scene->photons.end(), 0, scene);

}

void causticPistol() {
	if (viewport.causticPhotonsPerLight == 0) return;
	// shoot photons at the caustic thingies
	// don't want these to bounce...but how?
	vector<Photon*> causticAura;
	for (vector<Light*>::iterator it = scene->lights.begin(); it != scene->lights.end(); ++it) {
		Light* currentLight = *it;
		for (vector<Renderable*>::iterator ct = scene->caustics.begin(); ct != scene->caustics.end(); ++ ct) {
			Renderable* currentCaustic = *ct;
			vec3 photensity = (currentLight->power * currentLight->intensity) / viewport.causticPhotonsPerLight;
			for (int i = 0 ; i < viewport.causticPhotonsPerLight; i++) {
				vec4 photonDir = currentCaustic->randomSurfacePoint() - currentLight->pos;
				Photon* photon = new Photon(currentLight->pos, photonDir, photensity);
				causticAura.push_back(photon);
			}
		}
	}
	for (photIt phot = causticAura.begin(); phot != causticAura.end(); ++phot) {
		tracePhoton(*phot, max(PHOTCURSION-1,0));
	}

	scene->photonTree = new PhotonTree(scene->photons.begin(), scene->photons.end(), 0, scene);

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
	Camera* camera = scene->camera;
	#pragma omp parallel for shared(nextpercent)
	for (int x = 0; x < viewport.w; x++) {
		for (int y = 0; y < viewport.h; y++) {

			Ray camRay;
			vec3 color = vec3(0,0,0);
			float ax, ay;
			float randX = 0.0;
			float randY = 0.0;
			float divisor = 1.0 / (max(1,viewport.aliasing));
			for (ax = 0 ; ax < 1 ; ax +=divisor) {
				for (ay = 0 ; ay < 1 ; ay+=divisor) {
					if (viewport.jittery) {
						randX = (double)rand() * divisor / (double)RAND_MAX;
						randY = (double)rand() * divisor / (double)RAND_MAX;
					}
					float tmpX = (x+ax+randX)/viewport.w;
					float tmpY = (y+ay+randY)/viewport.h;
					camRay = camera->generate_ray(tmpX,tmpY);
					color += traceRay(camRay, 0);
				}
			}
			color = (1.0 / pow(max(1,viewport.aliasing),2.0)) * color;
			setPixel(x,y,color[0], color[1], color[2]);			

		}
		
		/* logging output */
		progress += 1.0;
		if (progress >= nextpercent) {	
			if (omp_get_thread_num() == 0)
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
	string sceneFile = "";
	for (int i=1; i<argc; i++) {
		arg = argv[i];
		if (arg.compare("-s") == 0 || arg.compare("--scene") == 0) {
			sceneFile = argv[++i];
		} else if (arg.compare("-pr")==0 || arg.compare("--print")==0) {
			imageWriter->fileName = argv[++i];
		} else if (arg.compare("-px") == 0 || arg.compare("--pixels")==0) {
			int width = atoi(argv[++i]);
			int height = atoi(argv[++i]);
			viewport.w = width;
			viewport.h = height;
			imageWriter->setSize(width, height);
		} else if (arg.compare("-a")==0 || arg.compare("--antialias")==0) {
			viewport.aliasing = atoi(argv[++i]);
		} else if (arg.compare("-j")==0) {
			viewport.jittery = true; 
		} else if (arg.compare("-ja")==0 || arg.compare("-aj")==0) {
			viewport.aliasing = atoi(argv[++i]);
			viewport.jittery = true;
		} else if (arg.compare("-ph")==0) {
			viewport.photons = true;
			viewport.photonsPerLight = atof(argv[++i]) * 1000;
		} else if (arg.compare("-e")==0) {
			viewport.gatherEpsilon = atof(argv[++i]);
		} else if (arg.compare("-c")==0) {
			viewport.causticPhotonsPerLight = atof(argv[++i]) * 1000;
		} else {
			Warning("Unrecognized command " + arg);
			Usage();
		}
	}
	if (sceneFile.length()>0) {
		scene->parseScene(sceneFile);	
	}
	
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	
	srand((unsigned)time(NULL));
	
	viewport = Viewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	imageWriter = new ImageWriter(viewport.w, viewport.h);
	scene = new Scene();

	processArgs(argc, argv);
	if (viewport.photons) {
		photonCannon();
		causticPistol();
	}
	render();

  	return 0;
}
