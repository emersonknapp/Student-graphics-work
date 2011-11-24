#ifndef _TREES_H
#define _TREES_H

#include <algorithm>
#include "pc.h"

#define LEAF_NUM_ELEMENTS 1

class Scene;
class Renderable;



/*
3-Dimensional tree
	stores midpoints in axis-aligned BSP Tree
*/
enum Axis { X, Y, Z, W };

typedef bool (*comp)(Renderable*, Renderable*);
bool xCompare(Renderable*, Renderable*);
bool yCompare(Renderable*, Renderable*);
bool zCompare(Renderable*, Renderable*);

class KDTree {
public:
	
	KDTree(vector<Renderable*>::iterator, vector<Renderable*>::iterator, int, Scene*);
	~KDTree();
	float rayIntersect();
	void print(int);


protected:
	bool leafNode;
	comp comparator;
	Scene* scene;
	vector<int> polygons;
	
	/* This is a binary tree, after all */
	KDTree* leftChild;
	KDTree* rightChild;
	
	/* The axis and splitting plane value */ 
	int axis;
	float median;
	/* Axis aligned bounding box of this branch of the tree */
	float aabb[6];
	
};



#endif

