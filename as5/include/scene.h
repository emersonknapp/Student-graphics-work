#ifndef _SCENE_H
#define _SCENE_H

#include "as5.h"
#include <map>


struct tri {
	int a,b,c;
} ;

struct quad {
	int a,b,c,d;
} ;


class Mesh {
public:
	vector<vec3> vertsVec;
	vector<vec3> normsVec;
	GLfloat* verts;
	GLfloat* norms;
	unsigned int* indices;
	int n_poly;
	
	string material;
	virtual void createArrays() = 0;
	void addVert(vec3);
	void addNorm(vec3);
	vec3 getVert(int);
	vec3 getNorm(int);
	LocalGeo bezpatchinterp(Mesh*, float, float);
	LocalGeo bezcurveinterp(vec3 curve[], float u);
	
};

class QuadMesh : public Mesh {
public:
	vector<quad> quadsVec;
	quad* quads;
	void createArrays();
	//void addVert(vec3);
	//void addNorm(vec3);
	//vec3 getVert(int);
	//vec3 getNorm(int);
	void addQuad(vec4);
	void addQuad(int,int,int,int);
	
	void uniformsubdividepatch(float);
};

class TriMesh : public Mesh {
	tri* tris;
	vector<vec2> uvValues;
	void createArrays();
//	void addVert(vec3);
//	void addNorm(vec3);
//	vec3 getVert(int);
//	vec3 getNorm(int);
	void addTri(vec3);
	void addTri(int, int, int);
	TriMesh getTriMesh(vec3*, vec2*, int&);
	void adaptivesubdividepatch(QuadMesh, float);
	void adaptivesubdividepatch(TriMesh, float);
};

class Scene {
public:
	Scene(string, float);
	void parseScene(string);
	void parseBez(string);
	bool parseBezLine(string);
	//bool parseLine(string);
	vec4 getVertex(int);
	int extractVertex(string);
	
	void update(float);
	
	vector<vec3> vertices;
	vector<QuadMesh*> quadmeshes;
	
	vec3 rotation, translation, scale;
	vec3 rotating, translating;
	vec3 ambience;
	
	map<string, Material*> materials;
	vector<Light*> lights;
	Camera * camera;
	
	int lastVertex;
	float param;
	bool smoothShading, wireframe, adaptiveSub;

};



#endif