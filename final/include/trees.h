#ifndef _TREES_H
#define _TREES_H

#include <algorithm>
#include "pc.h"

#define LEAF_NUM_ELEMENTS 3

class Scene;
class Renderable;
class Photon;

/*
3-Dimensional tree
	stores midpoints in axis-aligned BSP Tree
*/
enum Axis { X, Y, Z, W };

typedef bool (*rendComp)(Renderable*, Renderable*); //Renderable position comparison function pointer
bool rendCompareX(Renderable*, Renderable*);
bool rendCompareY(Renderable*, Renderable*);
bool rendCompareZ(Renderable*, Renderable*);
typedef bool (*photComp)(Photon*, Photon*); //Renderable position comparison function pointer
bool photCompareX(Photon*, Photon*);
bool photCompareY(Photon*, Photon*);
bool photCompareZ(Photon*, Photon*);



class KDTree {
public:
	
	KDTree(rendIt, rendIt, int, Scene*);
	KDTree();
	~KDTree();
	bool rayIntersect(Ray, float&, rendIt&);
	void print(int);
	void makeAABB();
	AABB* aabb;
protected:
	bool leafNode;
	rendComp comparator;
	Scene* scene;
	rendIt myBegin;
	rendIt myEnd;
	
	/* This is a binary tree, after all */
	KDTree* leftChild;
	KDTree* rightChild;
	
	/* The axis and splitting plane value */ 
	int axis;
	float median;
	
};

class distCompare {
	vec3 center;
	int radius;
public:
	distCompare(const vec3& c=vec3(0,0,0), const int& r=0) { center = c; radius = r;}

	bool operator() (const photIt& lhs, const photIt& rhs) const { 
		return  (((*lhs)->pos).dehomogenize()-center).length2() > (((*rhs)->pos).dehomogenize()-center).length2();
	}
};

class PhotonTree : protected KDTree {
public:
	PhotonTree(photIt, photIt, int, Scene*);
	~PhotonTree();
	bool gatherPhotons(AABB*, priority_queue<photIt,vector<photIt>,distCompare>&);
	void print(int);
	void makeAABB();
protected:
	PhotonTree* leftChild;
	PhotonTree* rightChild;
	photComp comparator;
	photIt myBegin;
	photIt myEnd;
	
};





#endif

