#include <algorithm>

class KDTree {
	KDTree();
	
};

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