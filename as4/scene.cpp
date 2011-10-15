#include "scene.h"

Scene::Scene(string filename) {
	parseScene(filename);
	Material parseMaterial;
	translation = vec3(0,0,0);
	scale = vec3(1,1,1);
	rotation = vec3(0,0,0);
}

vec3 Scene::getVertex(int i) {
	if (i >= 0) {
		return vertices[i];
	} else {
		return vertices[lastVertex+i];
	}
}

bool Scene::parseLine(string line) {
	

	string op;
	
	if (line.empty())
		return true;
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	ss >> op;
	if (op[0] == '#') {
		return true;
	} 
	else if (op.compare("v") == 0) {
		double x, y, z;
		ss >> x >> y >> z;
		vertices.push_back(vec3(x,y,z));
	} 
	else if (op.compare("f") == 0) {
		int i, j, k;
		ss >> i >> j >> k;
		//Do something with triangles
	} 
	else if (op.compare("ka") == 0) {
		float r, g, b;
		ss >> r >> g >> b;
		ambience = vec3(r, g, b);
		if (DEBUG) cout << "added ka = " << ambience << endl;
	}
	else if (op.compare("kd") == 0) {
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.kd = vec3(r, g, b);
		if (DEBUG) cout << "added kd = " << parseMaterial.kd << endl;				
	} 	
	else if (op.compare("ks") == 0) {
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.ks = vec3(r, g, b);
		if (DEBUG) cout << "added ks = " << parseMaterial.ks << endl;
	} 
	else if (op.compare("kr") == 0) {
		float r,g,b;
		ss >> r >> g >> b;
		parseMaterial.kr = vec3(r, g, b);
	}
	else if (op.compare("sp")==0) {
		int sp;
		ss >> sp;
		parseMaterial.sp = sp;
		if (DEBUG) cout << "added sp = " << parseMaterial.sp << endl;
	}	
	else if (op.compare("s")==0) { //Parse a sphere
		float r;
		ss >> r;
		Sphere* sph = new Sphere();
		sph->scale(r*scale);
		sph->rotate(rotation);
		sph->translate(translation);
		sph->material = parseMaterial;
		renderables.push_back(sph);
		if (DEBUG) cout << "Added sphere of radius " << r << " to scene." << endl;
		//cout << translation << rotation << scale << endl;
	} 
	else if (op.compare("t")==0) { //triangle i j k
		int i, j, k;
		ss >> i >> j >> k;
		vec4 a, b, c;
		a = vec4(getVertex(i), 1);
		b = vec4(getVertex(j), 1);
		c = vec4(getVertex(k), 1);
		Triangle* tri = new Triangle(a, b, c);
		tri->scale(scale);
		tri->rotate(rotation);
		tri->translate(translation);
		tri->material = parseMaterial;
		renderables.push_back(tri);
		if (DEBUG) cout << "Added triangle to scene." << endl;
	} 
	else if (op.compare("cam")==0) { //camera
		//cout << "Creating camera " << translation << rotation << scale << endl;
		camera = new Camera();
		camera->scale(scale);
		camera->rotate(rotation);
		camera->translate(translation);
	} 
	else if (op.compare("translate")==0) { //translate x y z
		if (DEBUG) cout << "Translating." << endl;
		float x,y,z;
		ss >> x >> y >> z;
		translation[0] = x;
		translation[1] = y;
		translation[2] = z;	
	}
	else if (op.compare("rotate")==0) { //rotate theta vec
		float x,y,z;
		ss >> x >> y >> z;
		rotation = vec3(x,y,z);
	} 
	else if (op.compare("scale")==0) { //scale x y z
		float x,y,z;
		ss>>x>>y>>z;
		scale = vec3(x,y,z);
	} 
	else if (op.compare("pl")==0) { //pointlight x y z r g b
		float x,y,z,r,g,b;
		ss >> x >> y >> z >> r >> g >> b;
		PLight* p = new PLight(vec4(x,y,z,1), vec3(r,g,b));
		lights.push_back(p);
	} 
	else if (op.compare("dl")==0) { //directionalight x y z r g 
		float x,y,z,r,g,b;
		ss >> x >> y >> z >> r >> g >> b;
		DLight* d = new DLight(vec4(x,y,z,0), vec3(r,g,b));
		lights.push_back(d);
	} 
	else if (op.compare("ct")==0) { //clear transformations
		translation = vec3(0,0,0);
		scale = vec3(1,1,1);
		rotation= vec3(0,0,0);
	} 
	else{
		cout << "Warning: unrecognized command " << op << endl;
	}
	if (ss.fail())
		return false;
	return true;
}

void Scene::parseScene(string filename) {
	ifstream inFile(filename.c_str(), ifstream::in);
	char line[1024];

	if (!inFile) {
		Error("Could not open file " + filename);
	}


	 while (inFile.good()) {
		inFile.getline(line, 1023);
		if(!parseLine(string(line))) {
			Error("Bad line in input file.");
		}
	}
	inFile.close();
}

bool Scene::rayIntersect(Ray r, float& t, int& index) {
	
	float newT;
	int renderableIndex=-1;
	bool hasHit = false;
	
	for (int i=0; i<renderables.size(); i++) {
		vec3 color = vec3(0,0,0);
		//cout << renderables[i]->tmat << endl << endl;
		if((newT=renderables[i]->ray_intersect(r)) < t && newT>0) {	
			hasHit = true;			
			index = i;
			t = newT;
		}
	}
	return hasHit;
}