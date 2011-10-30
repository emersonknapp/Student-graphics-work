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
	vector<vec2> uvVec;
	vector<tri> meshTriangles;
		
	//vector<vec3> controlPatch;
	
	GLfloat* verts;
	GLfloat* norms;
	unsigned int* indices;
	int n_poly;
	
	string material;
	virtual void createArrays() = 0;
	
	void addVert(vec3);
	void addNorm(vec3);
	void addUV(vec2);
	vec3 getVert(int);
	vec3 getNorm(int);
	vec2 getUV(int);
	
	LocalGeo bezpatchinterp(Mesh*, float, float);
	LocalGeo bezcurveinterp(vec3 curve[], float u);
	
	void uniformsubdividepatch(float);
	void adaptivesubdividepatch(float);	
	
	
};

class QuadMesh : public Mesh {
public:
	void createArrays();
};

class TriMesh : public Mesh {
public:
	void createArrays();
};

class Scene {
public:
	Scene(string, float, bool);
	void parseScene(string);
	void parseBez(string);
	bool parseBezLine(string, int);
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