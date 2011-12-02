#ifndef _SCENE_H
#define _SCENE_H

#include "pc.h"
#include <map>

class KDTree;
class PhotonTree;

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
	void earClip(string);	
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
	vector<Photon*> photons;
	Camera * camera;
	KDTree* kdTree;
	PhotonTree* photonTree;

	vec3 ambience; /* Ambient light color */
	
	//map<string, Material*> materials;
	

};
#endif
