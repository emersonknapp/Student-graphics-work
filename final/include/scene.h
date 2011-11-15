#ifndef _SCENE_H
#define _SCENE_H

#include "pc.h"
#include <map>

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
	Material parseMaterial;
	vector<vec4> vertices;
	map<string, Material*> materials;
	vector<Renderable*> renderables;
	vector<Light*> lights;
	Camera * camera;
	int lastVertex;
	vec3 ambience;

};


#endif