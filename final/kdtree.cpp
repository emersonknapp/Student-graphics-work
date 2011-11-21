#include <algorithm>
#include "scene.h"

class KDTree {
	KDTree();
	
};

/*
vertices for kdtree = one vector from each triangle, or centerpoints, also center of spheres
construct kdtree.
for each level of kdtree, construct corresponding AABB for the entire set.
Now, do intersections against AABB tree, the kdtree can wither and die.
*/

void KDTree::construct(vector<vec4>* vertices, int depth) {
	if (vertices->size() == 0) {
		return;
	} else {
		// Select axis based on depth so that axis cycles through all valid values
		int axis = depth % 3;

		// Sort point list and choose median as pivot element
		
		select median by axis from pointList;

		// Create node and construct subtrees
		var tree_node node;
		node.location := median;
		node.leftChild := kdtree(points in pointList before median, depth+1);
		node.rightChild := kdtree(points in pointList after median, depth+1);
		return node;
	}
}
