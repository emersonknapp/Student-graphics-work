//************************//
// Subdivision Code
//************************//

TriMesh getTriMesh(vector<vec4> q, int which) {
	TriMesh t;
	if (which == 0) {
		t.vertsVec.push_back(q[0]);
		t.vertsVec.push_back(q[1]);
		t.vertsVec.push_back(q[2]);
	} else {
		t.vertsVec.push_back(q[1]);
		t.vertsVec.push_back(q[2]);
		t.vertsVec.push_back(q[3]);
	}
}

TriMesh adaptivesubdividepatch(QuadMesh patch, float error) {
	//	assumes 16-point QuadMesh
	//	creates the 9 quadrilaterals 
	vector<vec4> quadrilaterals;
	vector<int> quad;
	for (int i = 0; i < 4; i += 1) {
		for (int j = 0; j < 4; j += 1) {
			if (quad.size() == 4) {
				quadrilaterals.push_back(vec4(quad[0],quad[1],quad[2],quad[3]));
				if (quadrilaterals.size()%2 == 0) quadrilaterals.pop_back();
				quad.empty();
			}
			quad.push_back(patch.getVert(i+4*j));
		}
	}
	//	loop through and create TriMeshes from each quadrilateral
	while (!quadrilaterals.empty()) {
		TriMesh t = getTriMesh(quadrilaterals[quadrilaterals.size()-1], quadrilaterals.size()%2);
		// now we pass these TriMeshes into adaptivesubdividepatch
	}
}

//given a patch, perform adaptive subdivision with triangles
TriMesh adaptivesubdividepatch(TriMesh patch, float error) {
	vec3 curve[3];
	vec3 splitEdges[3];
	vec3 trianglePoint[3];
	vec3 bezierPoint;
	// need to do the following for all 3 sides of the triangle
	// if the bezierPoint is some error away from the triangle midpoint
	//	then we create a new vertex, and split the triangle around that one
	// appropriately
	//	p0,p1 is edge 0
	//	p1,p2 is edge 1
	// 	p2,p0 is edge 2
	
	//TODO: want to iterate through some u,v for TriMesh
	//TODO: generate bezierPoint for each side of the Triangle
	
	trianglePoint[0] = 0.5f * (curve[0]+curve[1]);
	trianglePoint[1] = 0.5f * (curve[1]+curve[2]);
	trianglePoint[2] = 0.5f * (curve[2]+curve[0]);
	
	for (int i = 0; i < 3; i++) {
		if (error > (bezierPoint - trianglePoint).length2()) {
			splitEdges[i] = NULL;
		} else {
			splitEdges[i] = bezierPoint;
		}
	}
	// more stuff coming soon
}