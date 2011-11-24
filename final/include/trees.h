#ifndef _TREES_H
#define _TREES_H

/*
3-Dimensional tree
	stores midpoints in axis-aligned BSP Tree
*/
class KDTree {
	KDTree(vector<vec4>*, int);
	~KDTree();
	float rayIntersect
	
	vector<int> vertices;
	
	KDTree* leftChild;
	KDTree* rightChild;
	int median; //the value at this node
	float aabb[6];
	
	
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