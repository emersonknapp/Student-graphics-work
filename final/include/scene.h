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
	int extractVertex(string);
	bool rayIntersect(Ray, float&, int&);
	
	vec3 rotation, translation, scale;
	Material parseMaterial; /* Temporary storage for the most recently parsed material */
	vector<vec4> vertices;
	int lastVertex; /* Vertices.size()-1, shortcut to index of last vertex */
	vector<Renderable*> renderables;
	vector<Light*> lights;
	vector<Texture> textures;
	Camera * camera;
	KDTree* kdTree;

	vec3 ambience; /* Ambient light color */
	
	//map<string, Material*> materials;
	

};


#endif
