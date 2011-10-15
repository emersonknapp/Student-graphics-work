#include "as4.h"

class Scene {
public:
	void parseScene(string);
	bool parseLine(string);
	vector<vec3> vertices;
	map<string, Material*> materials;
	vector<int[3]> triangles;
	vector<Renderable*> renderables;
	Camera * camera;
	int lastVertex;
	vec3 ambience;

};

vec3 Scene::getVertex(int i) {
	if (i >= 0) {
		return vertices[i];
	} else {
		return vertices[lastVertex+i];
	}
}

void Scene::parseLine(string line) {
	string operator;
	if (line.empty())
		return true;
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	ss >> operator;
	if (operator[0] == '#') {
		return true;
	} 
	else if (operator.compare("v") == 0) {
		double x, y, z;
		ss >> x >> y >> z;
		vertices.push_back(vec3(x,y,z));
	} 
	else if (operator.compare("f") == 0) {
		int i, j, k;
		ss >> i >> j >> k;
		//Do something with triangles
	} 
	else if (operator.compare("ka") == 0) {
		float r, g, b;
		ss >> r >> g >> b;
		ambience = vec3(r, g, b);
		if (DEBUG) cout << "added ka = " << ambience << endl;
	}
	else if (operator.compare("kd") == 0) {
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.kd = vec3(r, g, b);
		if (DEBUG) cout << "added kd = " << parseMaterial.kd << endl;				
	} 	
	else if (operator.compare("ks") == 0) {
		float r, g, b;
		parseMaterial.ks = vec3(r, g, b);
		if (DEBUG) cout << "added ks = " << parseMaterial.ks << endl;
	} 
	else if (operator.compare("kr") == 0) {
		float r,g,b;
		ss >> r >> g >> b;
		parseMaterial.kr = vec3(r, g, b);
	}
	else if (operator.compare("sp")==0) {
		int sp;
		ss >> s;
		parseMaterial.sp = sp;
		if (DEBUG) cout << "added sp = " << parseMaterial.sp << endl;
	}	
	else if (operator.compare("s")==0) { //Parse a sphere
		float r;
		ss >> r;
		Sphere* sph = new Sphere();
		sph->scale(r*scale);
		sph->rotate(rotation);
		sph->translate(translation);
		sph->material = parseMaterial;
		renderables.push_back(sph);
		if (DEBUG) cout << "Added sphere of radius " << r << " to scene." << endl;
	} 
	else if (operator.compare("t")==0) { //triangle i j k
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
	else if (operator.compare("cam")==0) { //camera
		delete camera;
		camera = new Camera();
		camera->scale(scale);
		camera->rotate(rotation);
		camera->translate(translation);
	} 
	else if (operator.compare( "pr") { //print outputfile
		imageWriter.init(viewport.w, viewport.h);
		imageWriter.glOn = false;
		string name;
		ss >> name;
		//CHECKING WHAT HAPPENS WITH MISSING PARAMETER
		cout << name;
		imageWriter.fileName = name;
	} 
	else if (operator.compare( "translate") { //translate x y z
		float x,y,z;
		ss >> x >> y >> z;
		translation = vec3(x,y,z);				
	}
	if (ss.fail())
		return false;
	return true;
}


else if (operator.compare( "rotate") { //rotate theta vec
	for(int i=0; i<3; i++) {
		iss >> word;
		if (iss) {
			rotation[i] = atof(word.c_str());
		} else Error("Not enough arguments to rotate.");
	}
} 
else if (operator.compare( "scale") { //scale x y z
	for(int i=0; i<3; i++) {
		iss >> word;
		if (iss) {
			scale[i] = atof(word.c_str());
		} else Error("Not enough arguments to scale.");
	}
} 
else if (operator.compare( "pl") { //pointlight x y z r g b
	vec4 pos;
	vec3 color;
	for (int i=0; i<3; i++) {
		iss >> word;
		if (iss) {
			pos[i] = atof(word.c_str());
		} else Error("Not enough arguments to PointLight");
	}
	for (int i=0; i<3; i++) {
		iss >> word;
		if (iss) {
			color[i] = atof(word.c_str());
		} else Error("Not enough arguments to PointLight");
	}
	pos[3] = 1;
	PLight* p = new PLight(pos, color);
	plights.push_back(p);
	if (DEBUG) cout << "Added point light to scene." << endl;
} 
else if (operator.compare( "dl") { //directionalight x y z r g 
	vec4 dir;
	vec3 color;
	for (int i=0; i<3; i++) {
		iss >> word;
		if (iss) {
			dir[i] = atof(word.c_str());
		} else Error("Not enough arguments to Directional Light");
	}
	for (int i=0; i<3; i++) {
		iss >> word;
		if (iss) {
			color[i] = atof(word.c_str());
		} else Error("Not enough arguments to Directional Light");
	}
	dir[3] = 0;
	DLight* d = new DLight(dir, color);
	dlights.push_back(d);
	if (DEBUG) cout << "Added directional light to scene." << endl;
} 
else if (operator.compare( "ct"){ //clear transformations
	translation = vec3(0,0,0);
	scale = vec3(1,1,1);
	rotation= vec3(0,0,0);
} 
else{
	
}

void Scene::parseScene(string filename) {
	for (int i=1; i<argc; i++) {
		Material* usemtl;
		vec3 translation(0,0,0);
		vec3 scale(1,1,1);
		vec3 rotation(0,0,0);
	
		string arg = argv[i];
	
		ifstream inFile(arg.c_str(), ifstream::in);
		char line[1024];
	
		if (!inFile) {
			Error("Could not open file " + arg);
		}
	
	
		 while (inFile.good()) {
			inFile.getLine(line, 1023);
			if(!parseLine(string(line))) {
				Error("Bad line in input file " + arg);
			}
		}
		inFile.close();
	}
}