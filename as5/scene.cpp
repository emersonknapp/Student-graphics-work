x``#include "scene.h"

using namespace std;

Scene::Scene(string filename, float p, bool adapt) {
	param = p;
	adaptiveSub = adapt;
	parseBez(filename);
	for (int i=0; i<meshes.size(); i++) {
		if (adaptiveSub) {
			meshes[i]->adaptivesubdividepatch(param);
		} else {
			meshes[i]->uniformsubdividepatch(param);
		}
		meshes[i]->createArrays();
	}
	translation = vec3(0,0,-10);
	translating = vec3(0,0,0);
	scale = vec3(1,1,1);
	rotation = vec3(0,0,0);
	rotating = vec3(0,0,0);
	lastVertex = 0;
	smoothShading = true;
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
		if (adaptiveSub) {
			meshes.push_back(new TriMesh());
		} else {
			meshes.push_back(new QuadMesh());
		}
		for (int i=0; i<4; i++) {
			inFile.getline(l, 1023);
			line = string(l);
			if (line.empty()) i--;
			if(!parseBezLine(string(l), i)) {
				Error("Bad line in input file.");
			}	
		}
	}
	inFile.close();
}

bool Scene::parseBezLine(string line, int lineNum) {
	string a,b,c;
	if (line.empty())
		return true;
		
	stringstream ss(stringstream::in | stringstream::out);
	ss.str(line);
	for (int i=0; i<4; i++) {
		ss >> a >> b >> c;
		vec3 v = vec3(atof(a.c_str()), atof(b.c_str()), atof(c.c_str()));
		vec2 uv = vec2(i/3.0, lineNum/3.0);
		meshes.back()->addVert(v);
		//meshes.back()->addUV(uv);
		//cout << uv << endl;
	}
	return true;
	
}


void Mesh::addVert(vec3 v) {
	vertsVec.push_back(v);
}

void Mesh::addNorm(vec3 v) {
	normsVec.push_back(v);
}

void Mesh::addUV(vec2 v) {
	uvVec.push_back(v);
}
vec3 Mesh::getVert(int i) {
	return vertsVec[i];
}

vec3 Mesh::getNorm(int i) {
	return normsVec[i];
}

vec2 Mesh::getUV(int i) {
	return uvVec[i];
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
void Mesh::uniformsubdividepatch(float step) {
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
	n_poly = triangles.size();
	
	indices = new unsigned int[n_poly*3];
	for (int i=0; i<n_poly; i++) {
		int b = i*3;
		indices[b] = triangles[i].v[0];
		indices[b+1] = triangles[i].v[1];
		indices[b+2] = triangles[i].v[2];
	}
	
	//DEBUG PRINT
	cout << "VERTS" << endl;
	for (int i=0; i<vertsVec.size(); i++) {
		for (int j=0; j<3; j++) {
			cout << verts[i*3+j] << " ";
		}
		cout << endl;
	}
}

void Mesh::adaptivesubdividepatch(float error) {
	//assumes 16-point control mesh
	vec3		edges[3];
	vec2		uvs[3];
	LocalGeo 	actuals[3];
	vec3		edgeNorms[3];
	tri 		t;
	
	controlPatch = new QuadMesh();
	controlPatch->vertsVec = vertsVec;
	vertsVec.clear();
	
	//so I'm going to use the 2 triangle method
	LocalGeo p0 = bezpatchinterp(controlPatch, 0, 0);
	LocalGeo p1 = bezpatchinterp(controlPatch, 1, 0);
	LocalGeo p2 = bezpatchinterp(controlPatch, 0, 1);
	LocalGeo p3 = bezpatchinterp(controlPatch, 1, 1);
	
	addVert(p0.pos);
	addVert(p1.pos);
	addVert(p2.pos);
	addVert(p3.pos);
	addNorm(p0.dir);
	addNorm(p1.dir);
	addNorm(p2.dir);
	addNorm(p3.dir);
	addUV(vec2(0.0,0.0));
	addUV(vec2(1.0,0.0));
	addUV(vec2(0.0,1.0));
	addUV(vec2(1.0,1.0));
	
	tri t1 = {0, 1, 2};
	triangles.push_back(t1);
	
	tri t2 = {1, 3, 2};
	triangles.push_back(t2);
	
	
	for (int i = 0 ; i<triangles.size() ; i++) {
	// for each of the triangles, for each of the 3 sides, check the error
	// if the error is fine, then we push the vertices onto the patch
	// if error too big, then we subdivide and push the new triangles onto the vector.
		bool edgeOK[3];
		
		t = triangles[i];
		tri t1, t2, t3, t4;
		int numSplits = 0;
		
		for (int q=0; q<3; q++) {
			edges[q] = (getVert(t.v[q]) + getVert(t.v[(q+1)%3])) / 2.0;
			//cout << "Edge uvs: " << getUV(t.v[q]) << getUV(t.v[(q+1)%3]) << endl;
			//cout << "Edge: " << edges[q] << endl;
			uvs[q] = (getUV(t.v[q]) + getUV(t.v[(q+1)%3])) / 2.0;
			//cout << "UV: " << uvs[q] << endl;
			actuals[q] = bezpatchinterp(controlPatch, uvs[q][0], uvs[q][1]);
			//cout << "Actual: " << actuals[q].pos << endl;
			//cout << endl;
		}
		
		for (int j = 0 ; j < 3 ; j++) {
			LocalGeo g = actuals[j];
			if (error > (edges[j] - g.pos).length()) {
				edgeOK[j] = true;
			} else {
				edgeOK[j] = false;
				//edges[j] = g.pos;
				//edgeNorms[j] = g.dir;
				numSplits += 1;
			}
		}
		
		
		// now divide the triangles
		if (numSplits == 3) {
			cout << "All three sides are bad." << endl;
			cout << t.v[0] << " " << t.v[1] << " " << t.v[2] << endl << endl;
			
			int bottom = vertsVec.size();
			for (int k=0; k<3; k++) {
				addVert(actuals[k].pos);
				addNorm(actuals[k].dir);
				addUV(uvs[k]);
			}

			t1.v[0] = t.v[0];
			t1.v[1] = bottom;
			t1.v[2] = bottom+2;
			
			t2.v[0] = bottom;
			t2.v[1] = t.v[1];
			t2.v[2] = bottom+1;
			
			t3.v[0] = bottom;
			t3.v[1] = bottom+1;
			t3.v[2] = bottom+2;
			
			t4.v[0] = bottom+2;
			t4.v[1] = bottom+1;
			t4.v[2] = t.v[2];
			
			triangles.push_back(t1);
			triangles.push_back(t2);
			triangles.push_back(t3);
			triangles.push_back(t4);
			triangles.erase(triangles.begin()+i);	
			
		} else if (numSplits == 2) {
			
			if (edgeOK[0]) {
				
				t1.a = t.a;
				t1.b = t.b;
				t1.c = vertsVec.size()-numSplits+1;
				
				t2.a = t.a;
				t2.b = t1.c;
				t2.c = t1.c+1;
				
				t3.a = t2.c;
				t3.b = t2.b;
				t3.c = t.c;
				
				triangles.push_back(t1);
				triangles.push_back(t2);
				triangles.push_back(t3);
				triangles.erase(triangles.begin()+i);
				
			} else if (edgeOK[1]) {
				addVert(edges[0]);
				addNorm(edgeNorms[0]);
				addVert(edges[2]);
				addNorm(edgeNorms[2]);
				t1.a = t.a;
				t1.b = vertsVec.size()-numSplits;
				t1.c = t1.b+2;
				
				t2.a = t1.b;
				t2.b = t.b;
				t2.c = t1.c;
				
				t3.a = t1.c;
				t3.b = t.b;
				t3.c = t.c;
				
			} else if (edgeOK[2])) {
				addVert(edges[0]);
				addNorm(edgeNorms[0]);
				addVert(edges[1]);
				addNorm(edgeNorms[1]);
				t1.a = t.a;
				t1.b = vertsVec.size()-numSplits;
				t1.c = t.c;
				
				t2.a = t1.b;
				t2.b = t.b;
				t2.c = t1.b+1;
				
				t3.a = t1.b;
				t3.b = t1.b+1;
				t3.c = t.c;
			}
			
			t4.a = -1;
			t4.b = -1;
			t4.c = -1;
			
		} /*else if (numSplits == 1) {
			for (int j = 0; j < 3 ; j++) {
				if (edges[j] == edges[j]) { //checks that this edge needs to be split
					addVert(edges[j]);
					addNorm(edgeNorms[j]);
					t1.a = t.a;
					t1.b = vertsVec.size()-1; //split point is most recent vertex
					t1.c = t.a + (j+2)%3;

					t2.a = t.a + (j+1)%3;
					t2.b = t1.b;
					t2.c = t.c;
				} 
			}
			t3.a = -1;
			t3.b = -1;
			t3.c = -1;
			t4.a = -1;
			t4.b = -1;
			t4.c = -1;
		} else {
			t1.a = -1;
			t1.b = -1;
			t1.c = -1;
			t2.a = -1;
			t2.b = -1;
			t2.c = -1;
			t3.a = -1;
			t3.b = -1;
			t3.c = -1;
			t4.a = -1;
			t4.b = -1;
			t4.c = -1;
		}
		//loop through triangles t1,t4. If the triangle vertices aren't -1 (meaning we used that triangle) , then we push onto the TriMesh triangle vector
		//TODO: update uvVec for these triangles. I'm thinking that we push_back 3 vec2s of (u,v) values for each triangle
		// that way, triangles.size() * 3 = uvVec.size()
		if (t1.a != -1) {
			triangles.push_back(t1);
		}
		if (t2.a != -1) {
			triangles.push_back(t2);
		}
		if (t3.a != -1) {
			triangles.push_back(t3);
		}
		if (t4.a != -1) {
			triangles.push_back(t4);
		}
		*/
	}
	
	//when we're all done subdividing this patch, push all triangles onto triangles
	/*
	for (int i = 0; i < triangles.size(); i++ ) {
		triangles.push_back(triangles[i]);
	}
	*/
	
	cout << "end subdivide" << endl;
	
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
