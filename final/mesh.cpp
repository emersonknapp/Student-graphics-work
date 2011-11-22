#include "pc.h"

void Mesh::parse(string filename) {
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
}


bool Mesh::parseLine(string line) {	

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
	else if (op.compare("f") == 0) { //face, for now just a triangle TODO: earclipping
		int i, j, k;
		ss >> i >> j >> k;
		vec3& a = vertices[i];
		vec3& b = vertices[j];
		vec3& c = vertices[k];
		
		Triangle* tri = new Triangle(a, b, c);

		renderables.push_back(tri);
		if (DEBUG) cout << "Added triangle to scene. " << a << b << c << endl;
		
	} 
	else if (op.compare("s")==0) { //Parse a sphere
		float r;
		ss >> r;
		Sphere* sph = new Sphere();
		sph->scale(r);

		renderables.push_back(sph);
		if (DEBUG) cout << "Added sphere of radius " << r << " to scene." << endl;
		//cout << translation << rotation << scale << endl;
	}
	else{
		Warning("unrecognized command " + op);
	}
	if (ss.fail())
		return false;
	return true;
}