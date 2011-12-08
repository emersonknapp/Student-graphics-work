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

void Scene::earClip(string line) {


	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	string v;
	string dummy;
	vector<int> polygonVertices; // indices into vertices vector
	vector<int> polygonTextureVertices;
	vector<int> polygonVertexNormals;
	ss >> dummy;
	// get all vertices
	while (ss >> v)	{
		int l, vt, vn;
		vt = vn = -1;
		l = extractVertex(v, vt, vn);
		polygonVertices.push_back( l );
		polygonTextureVertices.push_back( vt );
		polygonVertexNormals.push_back( vn );
	}
	int numVertices = polygonVertices.size();
	vec3 prevVertex, nextVertex, curVertex;
	int prevIndex, nextIndex;
	int cur = 0;
	while (numVertices > 3) {
		// initialize other points of possible ear (centered at curVertex)
		// prev,nextVertex are indices into polygonVertices, which are in turn indices into the vertices vector
		prevIndex = (cur-1+numVertices) % numVertices;
		nextIndex = (cur+1) % numVertices;

		prevVertex = getVertex(polygonVertices[prevIndex]);
		nextVertex = getVertex(polygonVertices[nextIndex]);
		curVertex = getVertex(polygonVertices[cur]) ;

		vec4 side1 = prevVertex - curVertex; 
		vec4 side2 = nextVertex - curVertex;

		float cosangle = side1.dehomogenize().normalize() * side2.dehomogenize().normalize();
		// if angle < 180, then curVertex is a convex vertex
		float sinangle = sqrt ( 1.0 - pow(cosangle,2.0f)  ) ;
		if (sinangle > 0) { // sin < 0 means < 180degrees
			// check no other vertex from triangle is inside this ear
			for (int tmp = 0; tmp <= numVertices ; tmp++) {
				if (tmp != prevIndex and tmp != cur and tmp != nextIndex) {
					vec3 tmpVertex = vertices[ polygonVertices[tmp] ];
					vec3 tmpBary = barycentric(prevVertex, curVertex, nextVertex, tmpVertex);
					// check if barycentric coordinate is outside triangle (any one of its elements > 1)
					if (tmpBary[0] + tmpBary[1] + tmpBary[2] <=1) {
						// add the current ear to the renderables as a triangle, parse the texture vertices and shit
						// then delete the convex vertex!
						vec4 a, b, c, d, e, f, x, y, z;
						
						a = getVertex(polygonVertices[prevIndex]);
						b = getVertex(polygonVertices[cur]);
						c = getVertex(polygonVertices[nextIndex]);

						d = getTextureVertex(polygonTextureVertices[prevIndex]);
						e = getTextureVertex(polygonTextureVertices[cur]);
						f = getTextureVertex(polygonTextureVertices[nextIndex]);

						x = getVertexNormal(polygonVertexNormals[prevIndex]);
						y = getVertexNormal(polygonVertexNormals[cur]);
						z = getVertexNormal(polygonVertexNormals[nextIndex]);

						Triangle* tri = new Triangle(a, b, c, d, e, f, x, y, z);
						tri->scale(scale);
						tri->rotate(rotation);
						tri->translate(translation);
						tri->material = parseMaterial;

						renderables.push_back(tri);

						if (parseMaterial.ri > 0) caustics.push_back(tri);		

						//delete convexVertex
						polygonVertices.erase (polygonVertices.begin() + cur);
						polygonTextureVertices.erase (polygonTextureVertices.begin() + cur);
						polygonVertexNormals.erase (polygonVertexNormals.begin() + cur);
						numVertices = polygonVertices.size();
					} 
				}
			}
		}
		cur = (cur+1) % numVertices;
	}

	prevIndex = (cur-1+numVertices) % numVertices;
	nextIndex = (cur+1) % numVertices;

	vec4 a, b, c, d, e, f, x, y, z;
	
	a = getVertex(polygonVertices[prevIndex]);
	b = getVertex(polygonVertices[cur]);
	c = getVertex(polygonVertices[nextIndex]);

	d = getTextureVertex(polygonTextureVertices[prevIndex]);
	e = getTextureVertex(polygonTextureVertices[cur]);
	f = getTextureVertex(polygonTextureVertices[nextIndex]);

	x = getVertexNormal(polygonVertexNormals[prevIndex]);
	y = getVertexNormal(polygonVertexNormals[cur]);
	z = getVertexNormal(polygonVertexNormals[nextIndex]);
	
	Triangle* tri = new Triangle(a, b, c, d, e, f, x, y, z);
	tri->scale(scale);
	tri->rotate(rotation);
	tri->translate(translation);
	tri->material = parseMaterial;

	renderables.push_back(tri);
	if (parseMaterial.ri > 0) caustics.push_back(tri);	

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
		if ((ss>>c) == 0) c = 0; 
//		if (abs(a) > 1) Error("Texture vertices must be between 0 and 1"); 
//		if (abs(b) > 1) Error("Texture vertices must be between 0 and 1");
//		if (abs(c) > 1) Error("Texture vertices must be between 0 and 1");
		lastTextureVertex++;
		textureVertices.push_back(vec3(a,b,c));
		return true;
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

		earClip(line);
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
		if (parseMaterial.ri > 0) caustics.push_back(sph);		
		if (DEBUG) cout << "Added sphere of radius " << r << " to scene." << endl;
		//cout << translation << rotation << scale << endl;
	}
	else if (op.compare("usemtl") == 0) { // open .mtl file
		string f;
		ss >> f;
		parseMTL(f);
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
		float x,y,z,r,g,b,power;
		ss >> x >> y >> z >> r >> g >> b >> power;
		PLight* p = new PLight(vec4(x,y,z,1), vec3(r,g,b), power);
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
	
	 while (inFile.good()) {
		inFile.getline(line, 1023);
		if(!parseLine(string(line))) {
			Error("Bad line in input file:" + string(line));
			
		}
	}
	inFile.close();
	/*Contruct kdtree for this scene*/
	//delete kdTree;
	kdTree = new KDTree(renderables.begin(), renderables.end(), 0, this);
	//kdTree->print(0);
	
}

void Scene::parseMTL(string filename) {
	ifstream inFile(filename.c_str(), ifstream::in);
	char line[1024];

	if (!inFile) Error("Could not open file " + filename);

	while (inFile.good()) {
		inFile.getline(line, 1023);
		if(!getMTLLine(string(line))) Error("Bad line in material file.");
	}
	inFile.close();
}

bool Scene::getMTLLine(string line) {
	
	string op;
	
	if (line.empty())
		return true;
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	ss >> op;

	if (op.compare("ka") == 0) { //ambient
		float r, g, b;
		ss >> r >> g >> b;
		ambience = vec3(r, g, b);
	}
	else if (op.compare("kd") == 0) { //diffuse
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.kd = vec3(r, g, b);
	} 	
	else if (op.compare("ks") == 0) { //specular
		float r, g, b;
		ss >> r >> g >> b;
		parseMaterial.ks = vec3(r, g, b);
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
	}	
	else if (op.compare("ri")==0) { //index of refraction
		float ri;
		ss >> ri;
		parseMaterial.ri = ri;
	}
	else if (op.compare("tex")==0) { //set new texture map
		string tmp;
		ss >> tmp;
		const char* name = tmp.c_str();
		Texture t = Texture(name);
		parseMaterial.texture = t;
	}
	if (ss.fail())
		return false;
	return true;
}

bool Scene::rayIntersect(Ray r, float& t, int& index) {
	rendIt rend;
	bool hasHit = kdTree->rayIntersect(r, t, rend);
	if (hasHit) {
		index = distance(renderables.begin(), rend);
	}
	return hasHit;
	
}
