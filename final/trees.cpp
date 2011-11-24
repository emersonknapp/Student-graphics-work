#include "trees.h"

/*
vertices for kdtree = one vector from each triangle, or centerpoints, also center of spheres
construct kdtree.
for each level of kdtree, construct corresponding AABB for the entire set.
Now, do intersections against AABB tree, the kdtree can wither and die.
*/

bool xCompare(vec4 a, vec4 b) { return a[X] < b[X]; }
bool yCompare(vec4 a, vec4 b) { return a[Y] < b[Y]; }
bool zCompare(vec4 a, vec4 b) { return a[Z] < b[Z]; }

KDTree::KDTree(vector<vec4>::iterator begin, vector<vec4>::iterator end, int depth, Scene* s) {
	scene = s;
	if (begin == end) {
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
		vector<vec4>::iterator median = begin;
		advance(median, medianIndex);
		vec4 pivot = *median;
		cout << pivot << endl;
		//median = pivot[axis];
		
		// Construct subtrees
		//location = median;
		//leftChild = new KDTree(points in vertices before median, depth+1);
		//rightChild = new KDTree(points in vertices after median, depth+1);
	}
}

KDTree::~KDTree() {
	delete leftChild;
	delete rightChild;
}
