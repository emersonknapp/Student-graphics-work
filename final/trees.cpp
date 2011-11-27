#include "trees.h"

/*
vertices for kdtree = one vector from each triangle, or centerpoints, also center of spheres
construct kdtree.
each kdtree node has an aabb with it that intersection is tested against
*/

bool xCompare(Renderable* a, Renderable* b) { 
	return a->center[X] < b->center[X]; 
}
bool yCompare(Renderable* a, Renderable* b) { 
	return a->center[Y] < b->center[Y]; 
}
bool zCompare(Renderable* a, Renderable* b) { 
	return a->center[Z] < b->center[Z]; 
}

KDTree::KDTree(rendIt begin, rendIt end, int depth, Scene* s) {
	/*Defining instance vars */
	myBegin = begin;
	myEnd = end;
	leftChild = NULL;
	rightChild = NULL;
	scene = s;
	
	/*Construct the tree */
	if (distance(begin, end) <= LEAF_NUM_ELEMENTS) { 
		leafNode = true;
		leafMakeAABB();
		return;
	} else {
		leafNode = false;
		// Select axis based on depth so that axis cycles through all valid values
		axis = depth % 3;
		switch(axis) {
			case X:
				comparator = xCompare;
				break;
			case Y:
				comparator = yCompare;
				break;
			case Z:
				comparator = zCompare;
				break;
			default:
				Error("Received illegal axis in kdtree constructor.");
		}

		/* Sort point list and choose median as pivot element */
		sort(begin, end, comparator);
		int medianIndex = distance(begin, end)/2;
		rendIt medianIterator = begin;
		
		advance(medianIterator, medianIndex);
		vec3 pivot = (*medianIterator)->center;
		median = pivot[axis];
		
		// Construct subtrees
		leftChild = new KDTree(begin, medianIterator, depth+1, s);
		rightChild = new KDTree(medianIterator, end, depth+1, s);
	}
	
	//Construct AABB for non-leaf nodes
	for (; begin != end; ++begin) {
		//TODO: this
	}
	
}

float KDTree::rayIntersect(Ray r) {
	if (aabb->rayIntersect(r) >= 0) {
		float left = leftChild->rayIntersect(r);
		float right = rightChild->rayIntersect(r);
		return min(left, right);
	} 
	return -1;
}


KDTree::~KDTree() {
	delete leftChild;
	delete rightChild;
}

void KDTree::print(int indent) {
	if (leafNode) {
		cout << string(indent*2, ' ') << "LEAF NODE" << endl;
		rendIt begin = myBegin;
		for (; begin!=myEnd; begin++) {
			cout << string(indent*2, ' ') << (*begin)->center << endl;
		}
		return;
	}
	string ax = "";
	switch(axis) {
		case X:
			ax="X";
			break;
		case Y:
		ax="Y";
		break;
		case Z:
		ax = "Z";
		break;
		case W:
		ax = "W";
		break;
	}
	cout << string(indent*2, ' ');
	cout << "KDTree along axis " << ax << " " << median << endl;
	if (leftChild) {
		cout << string(indent*2+1, ' ') << "Left Child:" << endl;
		leftChild->print(indent+1);
	}
	if (rightChild) {
		cout << string(indent*2+1, ' ') << "Right Child:" << endl;
		rightChild->print(indent+1);
	}
	
}

void KDTree::leafMakeAABB() {
	
}
