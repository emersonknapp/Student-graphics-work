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
//	vec3 bezierPoint = bezcurveinterp(curve, 0.5f).point;
	// need to do the following for all 3 sides of the triangle
	// if the bezierPoint is *inside* the triangle, then subdivide
	// if it's outside, then we make a new triangle. This will have overlapping
	// triangles for a little bit, but the conflicting triangle should be subdivided
	// appropriately
	//	p0,p1 is edge 0
	//	p1,p2 is edge 1
	// 	p2,p0 is edge 2
	
	// want to iterate through some u,v for TriMesh
	
	trianglePoint[0] = 0.5f * (curve[0]+curve[1]);
	trianglePoint[1] = 0.5f * (curve[1]+curve[2]);
	trianglePoint[2] = 0.5f * (curve[2]+curve[0]);
	
	for (int i = 0; i < 3; i++) {
		if (error > (bezierPoint - trianglePoint).length2()) {
			splitEdges[i] = vec3(0,0,0);
		} else {
			splitEdges[i] = trianglePoint;
		}
	}
	// more stuff coming soon
}