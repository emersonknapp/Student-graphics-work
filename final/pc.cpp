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

void Usage() {
	cout << "Photon Cannon v" << VERSION_NUMBER << endl
	<< "Usage:" << endl
	<< "    pc -s scene [options]" << endl
	<< "        -s SCENEFILE: scene file" << endl
	<< "        -pr OUTFILE: output print file name, defaults to \"out.png\"" << endl
	<< "        -px X Y: the size of the output image, in pixels (default 800x800)" << endl
	<< "        -a N: antialiasing, n-by-n rays per pixel, defaults to 1" << endl
	<< "        [-ja | -aj] N: uses n^2 jittery antialiasing," << endl
    << "        -ph N: how many photons to shoot out (in thousands) (if not specified, defaults to 10)" << endl
    << "        -e N: epsilon for photon gathering" << endl
    << "        -c N: number of caustic photons (in thousands) (defaults to 0)" << endl
    << "        -r: displays raw photons" << endl
    << "        -i: displays indirect illumination only" << endl
    << "        -d: displays raytraced image + direct visualization of radiance from global photon map" << endl
    << "        -sh N: number of shadow rays to send out for checks, defaults to 1" << endl;
	quitProgram(0);
}

//Estimate of reflected radiance from x towards origin
vec4 radianceEstimate(vec4 origin, vec4 x, vec4 normal, Material* mat, PhotonTree* tree) {
	vec3 radiance = vec3(0);
	vec3 mins = x.dehomogenize() - vec3(viewport.gatherEpsilon);
	vec3 maxes = x.dehomogenize() + vec3(viewport.gatherEpsilon);
	AABB gatherBox = AABB(mins,maxes);
	priority_queue<photIt,vector<photIt>,distCompare> nearPhotons (distCompare(x, viewport.gatherEpsilon));
	
	int maxNeighbors = max(viewport.photonsPerLight * .005, 1.0);
	maxNeighbors = 500;
	double radius = viewport.gatherEpsilon;
	if (tree->gatherPhotons(&gatherBox,nearPhotons)) {
		int neighborsSoFar = 0;
		photIt neighbor;
		//cout << nearPhotons.size() << " ";
		while (neighborsSoFar <= maxNeighbors && !nearPhotons.empty()) {
			neighbor = nearPhotons.top();
			vec3 photColor = (*neighbor)->color;
			vec4 photDir = -(*neighbor)->dir;
			radiance += prod(mat->kd, photColor)*max((photDir*normal), 0.0);
			nearPhotons.pop();
			neighborsSoFar++;
		}
		if (neighborsSoFar < maxNeighbors) {
			radius = viewport.gatherEpsilon;
		} else {
			radius = ((*neighbor)->pos - x).length();
		}
	}
	//double distance = (x-origin).length();
	double area = PI*radius*radius;
	//double steradians = area / pow(distance, 2);
	radiance /= area;
	return radiance;	
}

vec3 shade(Ray, vec4, vec4, int, bool);
vec3 finalGather(vec4 x, vec4 omega, vec4 normal) {
	//integrate over incoming directions
		//brdf *
		//incoming light from incoming direction *
		//-direction * normal *
		//steradian
	int gather_rays = 100;
	vec3 color = vec3(0);
	Ray r;
	int renderableIndex=-1;
	float t = T_MAX;
	bool hasHit = false;
	vector<vec4> samples;
	//float steradians = 0;
	
	for (int i = 0; i < gather_rays; i++) {
		vec3 point = randomHemispherePoint(normal);
		vec4 diffuseRayDirection = vec4(point,0);
		//generate diffuse ray
		r = Ray(x+EPSILON*diffuseRayDirection, diffuseRayDirection);		
        t = T_MAX;
        renderableIndex = -1;
		hasHit = scene->rayIntersect(r, t, renderableIndex);	

		if (hasHit) {
			//color += vec3(1,0,0) / gather_rays;
			Renderable* rend = scene->renderables[renderableIndex];
			vec4 hitPoint = r.pos + t*r.dir;
			vec4 normal = rend->normal(hitPoint);
			Material mat = rend->material;
			//vec3 elseColor = shade(r, hitPoint, normal, renderableIndex, false);
			//vec3 elseColor = vec3(1,0,0);
			vec3 elseColor = radianceEstimate(x, hitPoint, normal, &mat, scene->photonTree);
			//cout << elseColor << endl;
			//cout << r.dir << r.pos << hitPoint << normal << renderableIndex << elseColor << endl;
			color +=  elseColor / gather_rays;
			
			
			//vec4 radiance = radianceEstimate(r.pos, hitPoint, normal, &mat, scene->photonTree);
			//steradians += radiance[3];
			//samples.push_back(radiance);
		}	
			
	}
    // a hemisphere has SA 2pi steradians. we get back in the variable 'steradians' the total steradians that we covered with the final gather
    // so what we need to do is normalize the surface area we *have* ( 'steradians' ) to the 2pi surface
	//float scalar = 2*PI/steradians;
	//float cos_theta =  r.dir*normal; //cosine term
	//for (size_t i=0; i < samples.size(); i++) {
	//	vec4 sample = samples[i];
	//	color += sample.dehomogenize()*scalar*sample[3]*cos_theta; 
	//}
	//if (color.length() >= 1.5) cout << "bc " << color << " ";
	return color;
}

//***************************************************
// Do phong shading on a point
//***************************************************
vec3 shade(Ray r, vec4 hitPoint, vec4 normal, int index, bool gather) {
	vec3 color = vec3(0,0,0); //Default black
	Renderable* rend = scene->renderables[index];
	
	Material material = rend->material;
	
	//Color =
		// Direct Illumination
		//+Specular Reflection
		//+Caustics
		//+Indirect Illumination
	
	//  Direct Illumination
	//+ Specular Reflection
	if (!viewport.indirectOnly || !gather)
	for (size_t i=0; i < scene->lights.size(); ++i) {
		Light* currentLight = scene->lights[i];
		vec3 thisLightColor = vec3(0);
		float shadowScalar = 1;
		
		float shadePixel = 0;
		float t = T_MAX;
		int renderableIndex=-1;
		
		//SHADOWS
		bool checkShadows = true;
		if (viewport.photons) {
			vec3 mins = hitPoint.dehomogenize() - vec3(viewport.gatherEpsilon);
			vec3 maxes = hitPoint.dehomogenize() + vec3(viewport.gatherEpsilon);
			AABB gatherBox = AABB(mins,maxes);
			priority_queue<photIt,vector<photIt>,distCompare> nearPhotons (distCompare(hitPoint, viewport.shadowEpsilon));
			checkShadows = scene->shadowHedge->gatherPhotons(&gatherBox,nearPhotons);
		}
		//send out shadow rays
		if (checkShadows) {
			for (int s=0; s < viewport.shadowRays; s++) {
				Ray lightCheck = Ray(hitPoint+EPSILON*normal, currentLight->shadowCheck(hitPoint));
				bool hasHit = scene->rayIntersect(lightCheck, t, renderableIndex);

				if (hasHit) {
					bool hitALight = scene->renderables[renderableIndex]->isLight();
					bool hitPastLight = ((currentLight->pos - hitPoint).length2() < (lightCheck.pos + t * lightCheck.dir - hitPoint).length2());
					bool noShadow = hitALight || hitPastLight;
					if (noShadow) shadePixel += 1;
				} else shadePixel += 1;
				t = T_MAX;
				renderableIndex = -1;
			}
			shadowScalar *= (shadePixel / viewport.shadowRays);
		} else {
			shadowScalar = 1;
			shadePixel = 1;
		}

		
		if (shadePixel > 0) {			
			vec3 lightColor = currentLight->intensity;
			vec4 lightVector = currentLight->lightVector(hitPoint+EPSILON*normal);
			lightVector.normalize();
			vec4 viewVector = r.pos-hitPoint;
			viewVector.normalize();
			vec4 reflectionVector = -lightVector + 2*(lightVector*normal)*normal;
			reflectionVector.normalize();
			//Diffuse term
			thisLightColor += prod(material.kd, lightColor)*max((lightVector*normal), 0.0);
			//Specular term
			vec3 specular = prod(material.ks, lightColor)*pow(max(reflectionVector*viewVector, 0.0), material.sp);
			thisLightColor += specular;	
		} else;
		
		thisLightColor *= shadowScalar;
		color += thisLightColor;
		
	}
	//Caustics
	//+Indirect Illumination
	if (viewport.photons && gather) {
		// Caustics
		if (viewport.causticPhotonsPerLight > 0) {
			//color += radianceEstimate(hitPoint, hitPoint, normal, &material, scene->causticBush);
		}
        if (!viewport.directRadiance) {
            vec3 gatherIn = finalGather(hitPoint, -r.dir, normal);
            color += gatherIn;
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
		Material mat = rend->material;		
		
		if (rend->isLight()) {
			if (normal * -r.dir > 0) {
				vec3 c = ((AreaLight*)rend)->intensity;
				c = c * (3/c.length2());
				return c;
			}
		}
		
		vec3 radiance;
		if (viewport.rawPhotons) {
			radiance = radianceEstimate(hitPoint, hitPoint, normal, &mat, scene->photonTree);
			radiance += radianceEstimate(hitPoint, hitPoint, normal, &mat, scene->causticBush);
			return radiance;
		}
		if (viewport.directRadiance) {
			radiance = radianceEstimate(hitPoint, hitPoint, normal, &mat, scene->photonTree);
			radiance += radianceEstimate(hitPoint, hitPoint, normal, &mat, scene->causticBush);
			color += radiance;
		}

		//Shade this point
		color += shade(r, hitPoint, normal, renderableIndex, true);
		
		vec3 n = -normal.dehomogenize();
		vec3 d = r.dir.dehomogenize();
		
		vec3 refl = d - 2*(d*n)*n;
		refl.normalize();
		
		// *******************************
		// COMPUTE REFRACTION
		if (mat.ri > 0) {
			double cos_theta_1, cos_theta_2, cos_theta_2_squared, n1, n2, negation;
			vec4 v_reflect, v_refract;
			cos_theta_1 = normal * -r.dir;			
			negation = cos_theta_1 > 0 ? 1 : -1;
			if (cos_theta_1 > 0) { 
				//Ray hits outside of object
				n1 = 1.0;
				n2 = mat.ri;
			} else {
				//Ray hits inside of object
				n1 = mat.ri;
				n2 = 1.0;
			}
			cos_theta_2_squared = 1 - (pow(n1/n2, 2)*(1-pow(cos_theta_1, 2)));
			if (cos_theta_2_squared > 0) {
				cos_theta_2 = sqrt(cos_theta_2_squared);
				v_reflect = r.dir + (2*cos_theta_1)*normal;
				v_refract = ((n1/n2)*r.dir) + negation*((n1/n2)*cos_theta_1 - cos_theta_2)*normal;
				v_refract.normalize();
				Ray refractRay = Ray(hitPoint + EPSILON*v_refract, v_refract);
				color += traceRay(refractRay, depth+1);
			} else {
				//Total internal reflection
			}
		}
		
		/// *******************************
		// COMPUTE REFLECTION
		vec3 kr = mat.kr;
		if (kr.length2() > 0) {
			vec4 reflDir = vec4(refl, 0);
			Ray reflRay = Ray(hitPoint+EPSILON*reflDir, reflDir);
			vec3 reflColor = traceRay(reflRay, depth+1);
			color += prod(kr,reflColor);
			color = reflColor;
		}		
		
		// *********************************
		// COMPUTE TEXTURE MAPPING
		if (rend->material.texture.exists) color += rend->textureColor(hitPoint);
	} 
	
	return color;
}

void tracePhoton(Photon* phot, int depth) {
	if (depth > PHOTCURSION) {
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
		vec4 normal = rend->normal(hitPoint);
				
		vec3 kd = mat.kd;
		vec3 ks = mat.ks;
		//******
		// Shadow Photons
		if (depth == 0) {
			Photon* shadowPhoton = new Photon(hitPoint+EPSILON*phot->dir, phot->dir, vec3(1));
			float shadowT = T_MAX;
			int tmpRenderable = -1;
			if (scene->rayIntersect(*shadowPhoton, shadowT, tmpRenderable)) {
				if (tmpRenderable == renderableIndex) {
					shadowPhoton->pos = shadowPhoton->pos + (shadowT+EPSILON) * shadowPhoton->dir;
					shadowT = T_MAX;
					scene->rayIntersect(*shadowPhoton, shadowT, tmpRenderable);
				}
				shadowPhoton->pos = shadowPhoton->pos + shadowT * shadowPhoton->dir;
				scene->shadowPhotons.push_back(shadowPhoton);
			}
		}
		
		
		float probReflect = max(max(kd[0]+ks[0], kd[1]+ks[1]), kd[2]+ks[2]);
		float randPick = rand01();
		
		if (randPick < probReflect) {
			vec4 normal = rend->normal(hitPoint);
			
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
				if (phot->caustic) scene->causticPhotons.push_back(phot);
				else scene->photons.push_back(phot);
				vec4 newDir = vec4(randomHemispherePoint(normal), 0);
				//hitPoint = phot->pos + EPSILON*vec4(newDir, 0);
				Photon* reflPhoton = new Photon(phot->pos+EPSILON*newDir, newDir, prod(kd, phot->color)/probDiffuseReflect);

				tracePhoton(reflPhoton, depth+1);
				
			} else {
				//Refraction
				if (mat.ri > 0) {
					double cos_theta_1, cos_theta_2, cos_theta_2_squared, n1, n2, negation;
					vec4 v_reflect, v_refract;
					cos_theta_1 = normal * -phot->dir;			
					negation = cos_theta_1 > 0 ? 1 : -1;
					if (cos_theta_1 > 0) { 
						//Ray hits outside of object
						n1 = 1.0;
						n2 = mat.ri;
					} else {
						//Ray hits inside of object
						n1 = mat.ri;
						n2 = 1.0;
					}
					cos_theta_2_squared = 1 - (pow(n1/n2, 2)*(1-pow(cos_theta_1, 2)));
					if (cos_theta_2_squared > 0) {
						cos_theta_2 = sqrt(cos_theta_2_squared);
						v_reflect = phot->dir + (2*cos_theta_1)*normal;
						v_refract = ((n1/n2)*phot->dir) + negation*((n1/n2)*cos_theta_1 - cos_theta_2)*normal;
						v_refract.normalize();
						phot->pos = hitPoint + EPSILON*v_refract;
						phot->dir = v_refract;
						phot->caustic = true;
						tracePhoton(phot, depth+1);
					} else {
						//Total internal reflection
					}
				}
				else {
					//Specular reflection
					phot->dir = vec4(refl, 0);
					phot->color = prod(ks, phot->color)/probSpecularReflect;
					tracePhoton(phot, depth+1);
				}
			}
		} else {
			//phot->color = prod(kd, phot->color);
			scene->photons.push_back(phot);
		}
	}
}

void photonCannon() {
	vector<Photon*> photonCloud;
	
	for (vector<Light*>::iterator it = scene->lights.begin(); it != scene->lights.end(); ++it) {
		Light* currentLight = *it;
		currentLight->generatePhotons(photonCloud, viewport.causticPhotonsPerLight + viewport.photonsPerLight, scene->kdTree->aabb);
	}
	//iterate through photonCloud, push photons that intersect onto scene->photons
	for (photIt phot = photonCloud.begin(); phot != photonCloud.end(); ++phot) {
		tracePhoton(*phot, 0);
	}
	//store photons that hit a renderable into kdtree
	scene->photonTree = new PhotonTree(scene->photons.begin(), scene->photons.end(), 0, scene);
}

void causticPistol() {
	// shoot photons at the caustic thingies
	// don't want these to bounce...but how?
	vector<Photon*> causticCloud;
	for (vector<Light*>::iterator it = scene->lights.begin(); it != scene->lights.end(); ++it) {
		Light* currentLight = *it;
		for (vector<Renderable*>::iterator ct = scene->caustics.begin(); ct != scene->caustics.end(); ++ ct) {
			Renderable* currentCaustic = *ct;
			float greatRadius = (currentLight->pos - vec4(currentCaustic->center,0)).length2();
			float ratio = currentCaustic->minorArea() / (4 * PI * greatRadius);
			vec3 photensity = ratio * (currentLight->power * currentLight->intensity) / (viewport.causticPhotonsPerLight+viewport.photonsPerLight);
			// scale photensity by ratio
			for (int i = 0 ; i < viewport.causticPhotonsPerLight; i++) {
				vec4 photonDir = currentCaustic->randomSurfacePoint() - currentLight->pos;
				Photon* photon = new Photon(currentLight->pos, photonDir, photensity);
				photon->caustic = true;
				causticCloud.push_back(photon);
			}
		}
	}
	for (photIt phot = causticCloud.begin(); phot != causticCloud.end(); ++phot) {
		tracePhoton(*phot, max(PHOTCURSION-1,0));
	}

	scene->causticBush = new PhotonTree(scene->causticPhotons.begin(), scene->causticPhotons.end(), 0, scene);

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
	//vector<string> sceneFiles;
	for (int i=1; i<argc; i++) {
		arg = argv[i];
		if (arg.compare("-s") == 0 || arg.compare("--scene") == 0) {
			scene->filesToParse.push_back(argv[++i]);
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
		} else if (arg.compare("-r")==0) {
			viewport.rawPhotons=true;
		} else if (arg.compare("-i")==0) {
			viewport.indirectOnly = true;
		} else if (arg.compare("-d")==0) {
			viewport.directRadiance = true;
		} else if (arg.compare("-sh")==0) {
			viewport.shadowRays = atoi(argv[++i]);
		} else if (arg.compare("-se")==0) {
			viewport.shadowEpsilon = atoi(argv[++i]);
		}
		else {
			Warning("Unrecognized command " + arg);
			Usage();
		}
	}
	scene->parseScene();
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
	
//	srand((unsigned)time(NULL));
	
	viewport = Viewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	imageWriter = new ImageWriter(viewport.w, viewport.h);
	scene = new Scene();

	processArgs(argc, argv);
	if (viewport.photons) {
		photonCannon();
		causticPistol();
		scene->shadowHedge = new PhotonTree(scene->shadowPhotons.begin(), scene->shadowPhotons.end(), 0, scene);
	}
	render();

  	return 0;
}
