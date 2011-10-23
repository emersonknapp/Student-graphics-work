#include "scene.h"

using namespace std;

Scene::Scene(string filename) {
	parseBez(filename);
	translation = vec3(0,0,0);
	scale = vec3(1,1,1);
	rotation = vec3(0,0,0);
	lastVertex = 0;
	adaptiveSub = false;
	//camera = new Camera();
}

vec4 Scene::getVertex(int i) {
	if (i >= 0) {
		return vertices[i-1];
	} else {
		return vertices[lastVertex+i];
	}
}

int Scene::extractVertex(string s) {
	string result = "";
	for (int i=0; i<s.length(); i++) {
		if (s[i] == '/') {
			break;
		} else {
			result += s[i];
		}
	}
	return atoi(result.c_str());
}

/*
bool Scene::parseLine(string line) {
	

	string op;
	
	if (line.empty())
		return true;
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	ss >> op;
	if (op.compare("")==0) {
		return true;
	}
	else if (op[0] == '#') {
		return true;
	}
	else if (op.compare("v") == 0) {
		double x, y, z;
		ss >> x >> y >> z;
		lastVertex++;
		vertices.push_back(vec4(x,y,z,1));
	} 
	else if (op.compare("f") == 0) {
		string i, j, k;
		ss >> i >> j >> k;
		int l, m, n;
		l = extractVertex(i);
		m = extractVertex(j);
		n = extractVertex(k);
		vec4 a, b, c;
		a = getVertex(l);
		b = getVertex(m);
		c = getVertex(n);
		Triangle* tri = new Triangle(a, b, c);
		tri->scale(scale);
		tri->rotate(rotation);
		tri->translate(translation);
		tri->material = parseMaterial;
		renderables.push_back(tri);
		if (DEBUG) cout << "Added triangle to scene. " << a << b << c << endl;
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
	else{
		cout << "Warning: unrecognized command " << op << endl;
	}
	if (ss.fail())
		return false;
	return true;
}
*/

void Scene::parseScene(string filename) {
	ifstream inFile(filename.c_str(), ifstream::in);
	char line[1024];

	if (!inFile) {
		Error("Could not open file " + filename);
	}


	 while (inFile.good()) {
		inFile.getline(line, 1023);
		//if(!parseLine(string(line))) {
		//	Error("Bad line in input file.");
		//}
	}
	inFile.close();
}

void Scene::parseBez(string filename) {
	meshes.push_back(new QuadMesh());
	ifstream inFile(filename.c_str(), ifstream::in);
	char l[1024];
	string line;
	int patches = 0;

	if (!inFile) {
		Error("Could not open file " + filename);
	}
	stringstream ss(stringstream::in | stringstream::out);
	
	if (inFile.good()) {
		inFile.getline(l, 1023);
		string num;
		line = string(l);
		if (!line.empty()) {
			ss.str(line);
			ss >> num;
			patches = atoi(num.c_str());
		}	
	}
	string coord;
	while (inFile.good()) {
		inFile.getline(l, 1023);
		if(!parseBezLine(string(l))) {
			Error("Bad line in input file.");
		}
	}
	inFile.close();
}

bool Scene::parseBezLine(string line) {

	string a,b,c;
	if (line.empty())
		return true;
		
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	for (int i=0; i<4; i++) {
		ss >> a >> b >> c;
		vec3 v = vec3(atof(a.c_str()), atof(b.c_str()), atof(c.c_str()));
		meshes[meshes.size()-1]->addVert(v);
	}
	return true;
	
}


void Mesh::addVert(vec3 v) {
	vertsVec.push_back(v);
}

void Mesh::addNorm(vec3 v) {
	normsVec.push_back(v);
}

vec3 Mesh::getVert(int i) {
	return vertsVec[i];
}

vec3 Mesh::getNorm(int i) {
	return normsVec[i];
}


void QuadMesh::createArrays() {
	
}

void QuadMesh::addQuad(vec4) {
	
}

void QuadMesh::addQuad(int,int,int,int) {
	
}
