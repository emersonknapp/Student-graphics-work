#ifndef _SCENE_H
#define _SCENE_H

#include "as4.h"
#include <map>

class Scene {
public:
	Scene(string);
	void parseScene(string);
	bool parseLine(string);
	vec3 getVertex(int);
	bool rayIntersect(Ray, float&, int&);
	
	vec3 rotation, translation, scale;
	Material parseMaterial;
	vector<vec3> vertices;
	map<string, Material*> materials;
	vector<Renderable*> renderables;
	vector<Light*> lights;
	Camera * camera;
	int lastVertex;
	vec3 ambience;

};


#endif