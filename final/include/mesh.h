#ifndef _MESH_H
#define _MESH_H

class Mesh {
public:
	vector<vec3> vertices;
	vector<Renderable*> renderables;
	void parse(string);
	bool parseLine(string);
	
	int lastVertex;

};

#endif