#include "scene.h"

using namespace std;

Scene::Scene() {
	translation = vec3(0,0,0);
	scale = vec3(1,1,1);
	rotation = vec3(0,0,0);
	lastVertex = 0;
	lastTextureVertex = 0;
}

vec4 Scene::getVertex(int i) {
	if (i >= 0) {
		return vertices[i-1];
	} else {
		return vertices[lastVertex+i];
	}
}

vec3 Scene::getTextureVertex(int i) {
	if (i >=0) {
		return textureVertices[i-1];
	} else {
		return vec3(-1,-1,-1); 
	}
}

vec4 Scene::getVertexNormal(int i) {
	if (i >= 0) {
		return vertexNormals[i-1];
	} else {
		return vec4(0,0,0,0);
	}
}

int Scene::extractVertex(string s, int &vt, int &vn) {
	string result = "";
	string vtres = "";
	string vnres = "";

	unsigned int i = 0;
	for (; i<s.length(); i++) {
		if (s[i] == '/') break;
		else result += s[i];
	}
	i++;
	for (; i<s.length(); i++) {
		if (s[i] == '/') break;
		else vtres += s[i];
	}
	i++;
	for (; i<s.length(); i++) {
		if (s[i] == '/') break;
		else vnres += s[i];
	}

	if (vnres=="") vn = -1; else vn = atoi(vnres.c_str());
	if (vtres=="") vt = -1; else vt = atoi(vtres.c_str());

	return atoi(result.c_str());

}

bool Scene::parseLine(string line) {
	

	string op;
	
	if (line.empty())
		return true;
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	ss >> op;
	if (op.compare("")==0) { //empty lines
		return true;
	}
	else if (op[0] == '#') { //comments
		return true;
	}
	else if (op.compare("v") == 0) { //vertex
		double x, y, z;
		ss >> x >> y >> z;
		lastVertex++;
		vertices.push_back(vec4(x,y,z,1));
	} 
	else if (op.compare("vt") == 0) { //texture vertex
		double a,b,c;
		ss >> a >> b;
		try {
			ss >> c;
		} catch (int e) {
			c = 0;
		}
//		if (abs(a) > 1) Error("Texture vertices must be between 0 and 1"); 
//		if (abs(b) > 1) Error("Texture vertices must be between 0 and 1");
//		if (abs(c) > 1) Error("Texture vertices must be between 0 and 1");
		lastTextureVertex++;
		textureVertices.push_back(vec3(a,b,c));
	}
	else if (op.compare("vn") == 0) { // vertex normals
		double a,b,c;
		ss >> a >> b >> c;
		lastVertexNormal++;
		vertexNormals.push_back(vec4(a,b,c,0));
	}
	else if (op.compare("f") == 0) { //face, for now just a triangle TODO: earclipping
		string i, j, k, zz;
		ss >> i >> j >> k;
		int l, m, n;
		int vt1, vt2, vt3;
		int vn1, vn2, vn3;
		vt1 = vt2 = vt3 = -1;
		vn1 = vn2 = vn3 = -1;
		l = extractVertex(i, vt1, vn1);
		m = extractVertex(j, vt2, vn2);
		n = extractVertex(k, vt3, vn3);
		// check if we need to parse texture vertices
		vec4 a, b, c;
		a = getVertex(l);
		b = getVertex(m);
		c = getVertex(n);
		vec3 d, e, f;
		d = getTextureVertex(vt1);
		e = getTextureVertex(vt2);
		f = getTextureVertex(vt3);
		vec4 x, y, z;
		x = getVertexNormal(vn1);
		y = getVertexNormal(vn2);
		z = getVertexNormal(vn3);
		Triangle* tri = new Triangle(a, b, c, d, e, f, x, y, z);
		tri->scale(scale);
		tri->rotate(rotation);
		tri->translate(translation);
		tri->material = parseMaterial;
		renderables.push_back(tri);
		if (DEBUG) cout << "Added triangle to scene. " << a << b << c << endl;
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
	else if (op.compare("ka") == 0) { //ambient
		float r, g, b;
		ss >> r >> g >> b;
		ambience = vec3(r, g, b);
		if (DEBUG) cout << "added ka = " << ambience << endl;
	}
	else if (op.compare("kd") == 0) { //diffuse
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.kd = vec3(r, g, b);
		if (DEBUG) cout << "added kd = " << parseMaterial.kd << endl;				
	} 	
	else if (op.compare("ks") == 0) { //specular
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.ks = vec3(r, g, b);
		if (DEBUG) cout << "added ks = " << parseMaterial.ks << endl;
	} 
	else if (op.compare("kr") == 0) { //reflective
		float r,g,b;
		ss >> r >> g >> b;
		parseMaterial.kr = vec3(r, g, b);
	}
	else if (op.compare("sp")==0) { //specular power
		int sp;
		ss >> sp;
		parseMaterial.sp = sp;
		if (DEBUG) cout << "added sp = " << parseMaterial.sp << endl;
	}	
	else if (op.compare("ri")==0) { //index of refraction
		float ri;
		ss >> ri;
		parseMaterial.ri = ri;
		if (DEBUG) cout << "added ri = " << parseMaterial.ri << endl;
	}
	else if (op.compare("cam")==0) { //camera
		//delete camera;
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
	else if (op.compare("tex")==0) { //set new texture map
		string tmp;
		ss >> tmp;
		const char* name = tmp.c_str();
		Texture t = Texture(name);
		parseMaterial.texture = t;
	}
	else if (op.compare("ctex")==0) { //clear texture map
		parseMaterial.texture = Texture();
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
	
	size_t f = filename.find(".obj");
	if (filename.size() - int(f) == 4 ) {
		//init Camera
		
		//init material
//		parseOBJ(inFile);
	}

	 while (inFile.good()) {
		inFile.getline(line, 1023);
		if(!parseLine(string(line))) {
			Error("Bad line in input file.");
		}
	}
	inFile.close();
	/*Contruct kdtree for this scene*/
	//delete kdTree;
	kdTree = new KDTree(renderables.begin(), renderables.end(), 0, this);
	//kdTree->print(0);
	
}

void Scene::parseOBJ(ifstream& obj) {
	char line[1024];
	string op;
	while (obj.good()) {
		obj.getline(line, 1023);
		
		stringstream ss(stringstream::in | stringstream::out);
		ss.str(line);
		ss >> op;
		
	}
	
	obj.close();
}

bool Scene::rayIntersect(Ray r, float& t, int& index) {
	rendIt rend;
	bool hasHit = kdTree->rayIntersect(r, t, rend);
	if (hasHit) {
		index = distance(renderables.begin(), rend);
	}
	return hasHit;
	
}
