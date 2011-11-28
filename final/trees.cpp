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
	makeAABB();
}


bool KDTree::rayIntersect(Ray r, float& t, rendIt& rend) {
	float newT;
	bool hasHit = false;
	if (leafNode) {
		rendIt begin = myBegin;
		for (; begin != myEnd; ++begin) {
			if ((newT = (*begin)->rayIntersect(r)) < t && newT>0) {
				hasHit = true;
				rend = begin;
				t = newT;
			}
		}
	} else { //Not a leaf node
		hasHit = leftChild->rayIntersect(r, t, rend);
		hasHit = rightChild->rayIntersect(r, t, rend);
	} 
	
	return hasHit;	
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
		default:
			Error("Unrecognized axis enumeration.");
	}
	cout << string(indent*2, ' ');
	cout << "KDTree along axis " << ax << " " << median << endl;
	aabb->print(indent);
	if (leftChild) {
		cout << string(indent*2+1, ' ') << "Left Child:" << endl;
		leftChild->print(indent+1);
	}
	if (rightChild) {
		cout << string(indent*2+1, ' ') << "Right Child:" << endl;
		rightChild->print(indent+1);
	}
	
}

void KDTree::makeAABB() {
	if (leafNode) {
		rendIt begin = myBegin;
		aabb = new AABB();
		for (; begin!=myEnd; ++begin) {
			aabb->concat((*begin)->makeAABB());
		}
	} else {
		aabb = new AABB();
		aabb->concat(leftChild->aabb);
		aabb->concat(rightChild->aabb);
	}
}

