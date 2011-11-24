#include "trees.h"

/*
vertices for kdtree = one vector from each triangle, or centerpoints, also center of spheres
construct kdtree.
for each level of kdtree, construct corresponding AABB for the entire set.
Now, do intersections against AABB tree, the kdtree can wither and die.
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

KDTree::KDTree(vector<Renderable*>::iterator begin, vector<Renderable*>::iterator end, int depth, Scene* s) {
	if (DEBUG) clog << "Constructing k-d tree depth " << depth << endl;
	
	scene = s;
	if (distance(begin, end) <= LEAF_NUM_ELEMENTS) { //TODO: leaf node behavior
		if (DEBUG) cout << "    LEAF NODE" << endl;
		if (DEBUG) clog << "k-d tree " << depth << " completed." << endl;
		return;
	} else {
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
		vector<Renderable*>::iterator medianIterator = begin;
		advance(medianIterator, medianIndex);
		vec3 pivot = (*medianIterator)->center;
		cout << "   Pivot: " << pivot << endl;
		median = pivot[axis];
		
		// Construct subtrees
		leftChild = new KDTree(begin, medianIterator, depth+1, s);
		rightChild = new KDTree(medianIterator, end, depth+1, s);
	}
	if (DEBUG) clog << "k-d tree " << depth << " completed." << endl;
}

KDTree::~KDTree() {
	delete leftChild;
	delete rightChild;
}
