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
	
	void uniformsubdividepatch(float);
	void adaptivesubdividepatch(float);	
	vector<tri> meshTriangles;	
	
};

class QuadMesh : public Mesh {
public:
	vector<quad> quadsVec;
	quad* quads;
	void createArrays();
};

class TriMesh : public Mesh {
	tri* tris;
	vector<vec2> uvValues;

	void createArrays();
	void addTri(vec3);
	void addTri(int, int, int);

};

class Scene {
public:
	Scene(string, float);
	void parseScene(string);
	void parseBez(string);
	bool parseBezLine(string);
	void build(string, float);
	//bool parseLine(string);
	vec4 getVertex(int);
	int extractVertex(string);
	
	void update(float);
	
	vector<vec3> vertices;
	vector<Mesh*> meshes;
	
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