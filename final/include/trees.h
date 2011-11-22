#ifndef _TREES_H
#define _TREES_H

/*
3-Dimensional tree
	stores midpoints in axis-aligned BSP Tree
*/
class KDTree {
	KDTree(vector<vec4>*, int);
	~KDTree();
	
	vector<vec3>* vertices;
	KDTree* leftChild;
	KDTree* rightChild;
	int median; //the value at this node
	
	
	
};

/* 
Axis-aligned bounding box tree
	Uses kdtree constructed from centers
*/
class AABB {
	AABB(KDTree* tree);
	~AABB();
	
};



#endif