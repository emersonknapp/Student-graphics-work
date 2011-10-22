#ifndef _SCENE_H
#define _SCENE_H

#include "as5.h"
#include <map>

class Scene {
public:
	Scene(string);
	void parseScene(string);
	void parseBez(string);
	//bool parseLine(string);
	vec4 getVertex(int);
	int extractVertex(string);
	
	vector<vec3> vertices;
	
	vec3 rotation, translation, scale;
	vec3 ambience;
	
	map<string, Material*> materials;
	vector<Light*> lights;
	Camera * camera;
	
	int lastVertex;
	float param;
	bool smoothShading, wireframe, adaptiveSub;

};

struct tri {
	int a,b,c;
} ;

struct quad {
	int a,b,c,d;
} ;


class Mesh {
	vector<vec3> vertsVec;
	vector<vec3> normsVec;
	float* verts;
	float* norms;
	int* indices;
	int n_poly;
	string material;
	virtual void createArrays();
	virtual void addVert(vec3) = 0;
	virtual void addNorm(vec3) = 0;
	virtual vec3 getVert(int) = 0;
	virtual vec3 getNorm(int) = 0;
};

/*
class TriMesh : public Mesh {
	tri* tris;
	void createArrays();
	void addVert(vec3);
	void addNorm(vec3);
	vec3 getVert(int);
	vec3 getNorm(int);
	void addTri(vec3);
	void addTri(int, int, int);
};
*/

class QuadMesh : public Mesh {
	quad* quads;
	void createArrays();
	void addVert(vec3);
	void addNorm(vec3);
	vec3 getVert(int);
	vec3 getNorm(int);
	void addQuad(vec4);
	void addQuad(int,int,int,int);
};


#endif