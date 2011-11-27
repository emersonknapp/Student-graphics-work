#ifndef _TREES_H
#define _TREES_H

#include <algorithm>
#include "pc.h"

#define LEAF_NUM_ELEMENTS 3

class Scene;
class Renderable;

/*
3-Dimensional tree
	stores midpoints in axis-aligned BSP Tree
*/
enum Axis { X, Y, Z, W };

typedef bool (*comp)(Renderable*, Renderable*);
typedef vector<Renderable*>::iterator rendIt;
bool xCompare(Renderable*, Renderable*);
bool yCompare(Renderable*, Renderable*);
bool zCompare(Renderable*, Renderable*);

class KDTree {
public:
	
	KDTree(rendIt, rendIt, int, Scene*);
	~KDTree();
	float rayIntersect(Ray);
	void print(int);
	void leafMakeAABB();


protected:
	AABB* aabb;
	bool leafNode;
	comp comparator;
	Scene* scene;
	vector<int> polygons;
	rendIt myBegin;
	rendIt myEnd;
	
	/* This is a binary tree, after all */
	KDTree* leftChild;
	KDTree* rightChild;
	
	/* The axis and splitting plane value */ 
	int axis;
	float median;
	
};



#endif

