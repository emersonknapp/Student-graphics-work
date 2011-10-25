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

void TriMesh::createArrays() {}

TriMesh TriMesh::getTriMesh(vec3* q, vec2* uv, int &which) {
	//the 'which' variable ensures that we get both triangles from the input quadrilateral
	TriMesh t;
	if (which == 0) {
		t.vertsVec.push_back(q[0]);
		t.vertsVec.push_back(q[1]);
		t.vertsVec.push_back(q[2]);
		
		t.uvValues.push_back(uv[0]);
		t.uvValues.push_back(uv[1]);
		t.uvValues.push_back(uv[2]);
		
		which = 1;
	} else {
		t.vertsVec.push_back(q[1]);
		t.vertsVec.push_back(q[2]);
		t.vertsVec.push_back(q[3]);
		
		t.uvValues.push_back(uv[1]);
		t.uvValues.push_back(uv[2]);
		t.uvValues.push_back(uv[3]);
	}
	return t;
}

TriMesh TriMesh::adaptivesubdividepatch(QuadMesh patch, float error) {
	//	assumes 16-point QuadMesh
	//	creates the 9 quadrilaterals 
	vector<vec3*> quadrilaterals;
	vector<vec2*> uvForQuad;
	vec3 quad[4];
	vec2 uvs[4];
	for (int i = 0; i < 4; i += 1) {
		for (int j = 0; j < 4; j += 1) {
			quad[j]=patch.getVert(i+4*j);
			uvs[j]=vec2(i/4.0,j/4.0); // u,v as canonical values
			if (j == 3) {
				quadrilaterals.push_back(quad); //if we have 4 values, push this quadrilateral 
				uvForQuad.push_back(uvs); // store a pair of u,v values for each vertex in the quadrilateral
			}
		}
	}
	//	loop through and create TriMeshes from each quadrilateral
	int which = 1;
	while (!quadrilaterals.empty()) {
		if (which == 0) {
			quadrilaterals.pop_back();
			uvForQuad.pop_back();
		}
		TriMesh t = getTriMesh(quadrilaterals[quadrilaterals.size()-1], uvForQuad[uvForQuad.size()-1], which);
		// now we pass these TriMeshes into adaptivesubdividepatch
		adaptivesubdividepatch(t,error);
	}
}

//given a patch, perform adaptive subdivision with triangles
TriMesh TriMesh::adaptivesubdividepatch(TriMesh patch, float error) {
	vec3 splitEdges[3];
	vec3 trianglePoint[3];
	vec3 bezierPoint[3];
	// need to do the following for all 3 sides of the triangle
	// if the bezierPoint is some error away from the triangle midpoint
	//	then we create a new vertex, and split the triangle around that one
	// appropriately
	//	p0,p1 is edge 0
	//	p1,p2 is edge 1
	// 	p2,p0 is edge 2
	
	trianglePoint[0] = 0.5f * (patch.vertsVec[0]+patch.vertsVec[1]);
	trianglePoint[1] = 0.5f * (patch.vertsVec[1]+patch.vertsVec[2]);
	trianglePoint[2] = 0.5f * (patch.vertsVec[2]+patch.vertsVec[0]);
	
	bezierPoint[0] = bezpatchinterp(&patch,patch.uvValues[0][0],patch.uvValues[0][1]).pos;
	bezierPoint[1] = bezpatchinterp(&patch,patch.uvValues[1][0],patch.uvValues[1][1]).pos;
	bezierPoint[2] = bezpatchinterp(&patch,patch.uvValues[2][0],patch.uvValues[2][1]).pos;
	
	for (int i = 0; i < 3; i++) {
		if (error > (bezierPoint[i] - trianglePoint[i]).length2()) {
			splitEdges[i] = NULL;
		} else {
			splitEdges[i] = bezierPoint[i];
		}
	}
	
	// loop through the splitEdges and create new triangles based on that
}


//given a control patch and (u,v) values, find the surface point and normal
LocalGeo Mesh::bezpatchinterp(Mesh* patch, float u, float v) {
	vec3 vcurve[4];
	vec3 ucurve[4];
	vec3 cc[4][4]; //takes the points from patch and groups them appropriately for bezcurveinterp
	LocalGeo gv, gu;
	LocalGeo greturn;

	//build control point for a Bezier curve in v
	for (int i =0; i<4; i++) {
	  for (int j=0; j<4; j++) {
		   cc[i][j] = patch->getVert(i+j*4);
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
