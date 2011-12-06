#include "trees.h"

/*
vertices for kdtree = one vector from each triangle, or centerpoints, also center of spheres
construct kdtree.
each kdtree node has an aabb with it that intersection is tested against
*/

bool rendCompareX(Renderable* a, Renderable* b) { 
	return a->center[X] < b->center[X]; 
}
bool rendCompareY(Renderable* a, Renderable* b) { 
	return a->center[Y] < b->center[Y]; 
}
bool rendCompareZ(Renderable* a, Renderable* b) { 
	return a->center[Z] < b->center[Z]; 
}
bool photCompareX(Photon* a, Photon* b) {
	return a->pos[X] < b->pos[X];
}
bool photCompareY(Photon* a, Photon* b) {
	return a->pos[Y] < b->pos[Y];
}
bool photCompareZ(Photon* a, Photon* b) {
	return a->pos[Z] < b->pos[Z];
}

PhotonTree::PhotonTree(photIt begin, photIt end, int depth, Scene* s) {
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
				comparator = photCompareX;
				break;
			case Y:
				comparator = photCompareY;
				break;
			case Z:
				comparator = photCompareZ;
				break;
		}

		/* Sort point list and choose median as pivot element */
		sort(begin, end, comparator);
		int medianIndex = distance(begin, end)/2;
		photIt medianIterator = begin;
		
		advance(medianIterator, medianIndex);
		vec3 pivot = (*medianIterator)->pos;
		median = pivot[axis];
		
		// Construct subtrees
		leftChild = new PhotonTree(begin, medianIterator, depth+1, s);
		rightChild = new PhotonTree(medianIterator, end, depth+1, s);		
	}
	makeAABB();
}
void PhotonTree::makeAABB() {
	aabb = new AABB();
	if (leafNode) {
		photIt begin = myBegin;
		for (; begin!=myEnd; ++begin) {
			aabb->concat((*begin)->pos);
		}
	} else {
		aabb->concat(leftChild->aabb);
		aabb->concat(rightChild->aabb);
	}
}

bool withinSphere(vec3 point, float radius, vec3 center) {
	return (point-center).length() <= radius;
}


bool PhotonTree::gatherPhotons(AABB* hitPoint, vector<photIt>& photons) {
	//TODO (EMERSON): it looks like when we run this on scene6, we only get intersections at the left corners (x ~ -10, y = -10/10)
	if (aabb->intersect(hitPoint)) {
		if (leafNode) {
			float radius = (hitPoint->maxes[0] - hitPoint->mins[0])/2.0;
			vec3 center = hitPoint->maxes - vec3(radius, radius, radius);
			for (photIt it = myBegin; it != myEnd; ++it) {
				if (withinSphere((*it)->pos, radius, center)) {
					photons.push_back(it);
				}
			}
		} else {
			leftChild->gatherPhotons(hitPoint, photons);
			rightChild->gatherPhotons(hitPoint, photons);
		}
	}
	return (photons.size() > 0);	
}

void PhotonTree::print(int indent) {
	if (leafNode) {
		aabb->print(indent);
		cout << string(indent*2, ' ') << "LEAF NODE" << endl;
		photIt begin = myBegin;
		for (; begin!=myEnd; begin++) {
			cout << string(indent*2, ' ') << (*begin)->pos << endl;
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
	cout << "PhotonTree along axis " << ax << " " << median << endl;
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

KDTree::KDTree() {
	
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
				comparator = rendCompareX;
				break;
			case Y:
				comparator = rendCompareY;
				break;
			case Z:
				comparator = rendCompareZ;
				break;
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
	if (aabb->rayIntersect(r)) {
		if (leafNode) {
			rendIt begin = myBegin;
			for (; begin != myEnd; ++begin) {
				newT = (*begin)->rayIntersect(r);
				if (newT < t && newT>0) {
					hasHit = true;
					rend = begin;
					t = newT;
				}
			}
		} else { //Not a leaf node
			bool rightHit = rightChild->rayIntersect(r, t, rend);
			bool leftHit = leftChild->rayIntersect(r, t, rend);
			hasHit = rightHit || leftHit;
		}
	}
	
	return hasHit;	
}

KDTree::~KDTree() {
	delete leftChild;
	delete rightChild;
}

void KDTree::print(int indent) {
	if (leafNode) {
		aabb->print(indent);
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

