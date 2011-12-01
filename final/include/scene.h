#ifndef _SCENE_H
#define _SCENE_H

#include "pc.h"
#include <map>

class KDTree;

class Scene {
public:
	Scene();
	void parseScene(string);
	void parseOBJ(ifstream&);
	bool parseLine(string);
	vec4 getVertex(int);
	vec3 getTextureVertex(int);
	vec4 getVertexNormal(int);
	int extractVertex(string, int&, int&);
	bool rayIntersect(Ray, float&, int&);
	
	vec3 rotation, translation, scale;
	Material parseMaterial; /* Temporary storage for the most recently parsed material */
	vector<vec4> vertices;
	vector<vec3> textureVertices;
	vector<vec4> vertexNormals;
	int lastVertex; /* Vertices.size()-1, shortcut to index of last vertex */
	int lastTextureVertex;
	int lastVertexNormal;
	vector<Renderable*> renderables;
	vector<Light*> lights;
	Camera * camera;
	KDTree* kdTree;
	//KDTree* photonTree;

	vec3 ambience; /* Ambient light color */
	
	//map<string, Material*> materials;
	

};
//TODO: earclipping!
// we loop around the vertices of the polygon. For every vertex V_i, we have
// triangle V_(i-1), V_i, V_(i+1). if the interior angle (angle between V_(i-1) - V_i and
// V_(i+1) - V_i ) is < 180, then V_i is a convex vertex. So for each of these triangles, if 
// V_i is a convex vertex and for the triangle no other vertex lies inside, then the triangle is
// an ear

#endif
