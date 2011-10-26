#include "scene.h"

using namespace std;

Scene::Scene(string filename, float p) {
	param = p;
	parseBez(filename);
	for (int i=0; i<quadmeshes.size(); i++) {
		quadmeshes[i]->uniformsubdividepatch(param);
		quadmeshes[i]->createArrays();
	}
	translation = vec3(0,0,-10);
	translating = vec3(0,0,0);
	scale = vec3(1,1,1);
	rotation = vec3(0,0,0);
	rotating = vec3(0,0,0);
	lastVertex = 0;
	adaptiveSub = false;
	smoothShading = true;
	//camera = new Camera();
}

void Scene::update(float dt) {
	translation = translation + (translating*dt);
	rotation = rotation + (rotating*dt);
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
			if (DEBUG) cout << "Number of patches: " << patches << endl;
		}	
	}
	string coord;
	for (int q=0; q<patches; q++) {
		quadmeshes.push_back(new QuadMesh());
		for (int i=0; i<4; i++) {
			inFile.getline(l, 1023);
			line = string(l);
			if (line.empty()) i--;
			if(!parseBezLine(string(l))) {
				Error("Bad line in input file.");
			}	
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
		quadmeshes.back()->addVert(v);
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
	if (vertsVec.size() != normsVec.size()) {
		Error("Improperly formed QuadMesh.");
	}
	int size = vertsVec.size()*3;
	verts = new GLfloat[size];
	norms = new GLfloat[size];
	for (int b=0; b<vertsVec.size(); b++) {
		int i = b*3;
		verts[i] = vertsVec[b][0];
		verts[i+1] = vertsVec[b][1];
		verts[i+2] = vertsVec[b][2];
		norms[i] = normsVec[b][0];
		norms[i+1] = normsVec[b][1];
		norms[i+2] = normsVec[b][2];
	}
	
	int linelength = int(sqrt(vertsVec.size()));
	
	
	int numquads = (linelength-1)*(linelength-1);
	n_poly = numquads;
	indices = new unsigned int[numquads*4];
	
	for (int i=0; i<numquads; i++) {
		unsigned int x = i%(linelength-1);
		unsigned int y = floor(1.0*i/(linelength-1));
		
		unsigned int start = x + y*linelength;
		int q = i*4;
		indices[q] = start;
		indices[q+1] = start+1;
		indices[q+2] = start+linelength+1;
		indices[q+3] = start+linelength;
	}
	
}

//Subdivide a control patch in place. If already subdivided, does nothing.
void QuadMesh::uniformsubdividepatch(float step) {
	//compute how many subdivisions there are for this step size
	if (vertsVec.size() > 16) {
		return;
	}
	float numdiv = int(1.0/step);
	
	float stepsize = 1/numdiv;

	float u,v;
	vector<LocalGeo> newInfo;
	//for each parametric value of u
	for (int iu = 0; iu <= numdiv; iu++) {
		u = iu*stepsize;
		//for each parametric value of v
		for (int iv = 0; iv<=numdiv;iv++) {
			v = iv*stepsize;
			//evaluate surface
			//printf("Calculating point (%f, %f)\n", u, v);
			LocalGeo g = bezpatchinterp(this,u,v);
			//printf("=(%f, %f, %f)\n", g.pos[0], g.pos[1], g.pos[2]);
			newInfo.push_back(g);
			//addVert(g.pos);
			//addNorm(g.dir);
		}
	}
	vertsVec = vector<vec3>();
	normsVec = vector<vec3>();
	for (int a=0; a<newInfo.size(); a++) {
		addVert(newInfo[a].pos);
		addNorm(newInfo[a].dir);
	}
}

//TODO: TriMesh::createArrays() --- similar to the QuadMesh one
void TriMesh::createArrays() {
	if (vertsVec.size() != normsVec.size()) {
		Error("Improperly formed TriMesh.");
	}
	int size = vertsVec.size()*3;
	verts = new GLfloat[size];
	norms = new GLfloat[size];
	for (int b=0; b<vertsVec.size(); b++) {
		int i = b*3;
		verts[i] = vertsVec[b][0];
		verts[i+1] = vertsVec[b][1];
		verts[i+2] = vertsVec[b][2];
		norms[i] = normsVec[b][0];
		norms[i+1] = normsVec[b][1];
		norms[i+2] = normsVec[b][2];
	}
	
	int linelength = int(sqrt(vertsVec.size()));
	
	
	int numtris = (linelength-1)*(linelength-1);
	n_poly = numtris;
	indices = new unsigned int[numtris*3];
	
	for (int i=0; i<numtris; i++) {
		unsigned int x = i%(linelength-1);
		unsigned int y = floor(1.0*i/(linelength-1));
		
		unsigned int start = x + y*linelength;
		int q = i*4;
		indices[q] = start;
		indices[q+1] = start+1;
		indices[q+2] = start+linelength+1;
		indices[q+3] = start+linelength;
	}
}

void TriMesh::adaptivesubdividepatch(QuadMesh patch, float error) {
	//	assumes 16-point QuadMesh
	vector<tri> triangles;
	tri t;
	for (int i = 0; i < 12; i += 1) {		//loop through the patch control points, pushing tris onto the Triangle vector
		if (i%2 == 0) {
			t->a = i;
			t->b = i+4;
			t->c = i+1;
		} else {
			t->a = i;
			t->b = i+3;
			t->c = i+4;
		}
		triangles.push_back(t);
	}
	for (int i = 0 ; i<triangles.size() ; i++) {
	// for each of the triangles, for each of the 3 sides, check the error
	// if the error is fine, then we push the vertices onto the patch
	// if error too big, then we subdivide and push the new triangles onto the vector
		t = triangles[i];
		
		//edges[0] = (getVert(t->a) + getVert(t->b)) * .5
		//edges[1] = '' (t->b,t->c)
		//edges[2] = '' (t->c,t->b)
		//check error > (edges[0] - bezpatchinterp(patch,u,v)) - where do we get u,v? from t->a,b
	}
}


//given a control patch and (u,v) values, find the surface point and normal
LocalGeo Mesh::bezpatchinterp(Mesh* patch, float u, float v) {
	vec3 vcurve[4];
	vec3 ucurve[4];
	vec3 cc[4][4]; //takes the points from patch and groups them appropriately for bezcurveinterp
	vec3 tmpvcurve[4];
	LocalGeo gv, gu;
	LocalGeo greturn;

	//build control point for a Bezier curve in v

	for (int i =0; i<4; i++) {
	  for (int j=0; j<4; j++) {
		   cc[i][j] = patch->getVert(i+j*4);
//		cout << "i=" << i << " j= " << j << " is " << cc[i][j] << endl;		
	  }
	}
	for (int j=0; j<4; j++) {
		vcurve[j] = bezcurveinterp(cc[j], u).pos; 
	}

	//build control points for a Bezier curve in u
	for (int i=0; i<4; i++) {
	  for (int j=0; j<4; j++) {
		   cc[i][j] = patch->getVert(i*4+j);

	  }
	}
	for (int j=0; j<4; j++) {
		ucurve[j] = bezcurveinterp(cc[j], v).pos;
	} 

	//evaluate surface and derivative for u and v
	gv = bezcurveinterp(vcurve, v);
	gu = bezcurveinterp(ucurve, u);
	//printf("	 point on v(%f): (%f, %f, %f)\n", v, gv.point[0], gv.point[1], gv.point[2]);
	//printf("	 point on u(%f): (%f, %f, %f)\n", u, gu.point[0], gu.point[1], gu.point[2]);

	//take cross product of partials to find normal
	
	vec3 n = gv.dir ^ gu.dir;
	if (vcurve[0] == vcurve[1] == vcurve[2] == vcurve[3]) {	
		n = (cc[1][1]-cc[1][3]) ^ (cc[3][1] - cc[3][3]);
	}
	if (n == vec3(0,0,0)) {
		n = (cc[1][3]-cc[1][1]) ^ (cc[3][3] - cc[3][1]);
	}
	n = n.normalize();	
	greturn.pos = gu.pos;
	greturn.dir = n;
	greturn.u = u;
	greturn.v = v;

	return greturn;

}

LocalGeo Mesh::bezcurveinterp(vec3 curve[], float u) {
	   vec3 A,B,C,D,E,p;
	   //first, split each of the three segments to form two new ones AB and BC
	   A = curve[0] * (1.0-u) + curve[1]*u;
	   B = curve[1] * (1.0-u) + curve[2]*u;
	   C = curve[2] * (1.0-u) + curve[3]*u;
	   //now, split AB and BC to form new segment DE
	   D = A * (1.0-u) + B * u;
	   E = B * (1.0-u) + C * u;
	   
	   //finally, pick the right point on DE, this is the point on the curve
	   p = D*(1.0-u)+E*u;
	   //compute the derivate also
	   vec3 dPdu = 3*(E-D);
	   LocalGeo greturn;
	   greturn.pos = p;
	   greturn.dir = dPdu;
	   return greturn;
}
