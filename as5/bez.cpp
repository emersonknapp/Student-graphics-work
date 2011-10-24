//************************//
// Subdivision Code
//************************//

TriMesh getTriMesh(vector<vec4> q, vector<vec2> uv, int &which) {
	TriMesh t;
	if (which == 0) {
		t.vertsVec.push_back(q[0]);
		t.vertsVec.push_back(q[1]);
		t.vertsVec.push_back(q[2]);
		
		t.uvValues.push_back(uv[0]);
		t.uvValues.push_back(uv[1]);
		t.uvValues.push_back(uv[2]);
		
		which = 1;
	} else {
		t.vertsVec.push_back(q[1]);
		t.vertsVec.push_back(q[2]);
		t.vertsVec.push_back(q[3]);
		
		t.uvValues.push_back(uv[1]);
		t.uvValues.push_back(uv[2]);
		t.uvValues.push_back(uv[3]);
	}
}

TriMesh adaptivesubdividepatch(QuadMesh patch, float error) {
	//	assumes 16-point QuadMesh
	//	creates the 9 quadrilaterals 
	vector<vec4> quadrilaterals;
	vector<vec2> uvForQuad;
	vector<int> quad;
	vector<vec2> uvs;
	for (int i = 0; i < 4; i += 1) {
		for (int j = 0; j < 4; j += 1) {
			if (quad.size() == 4) {
				quadrilaterals.push_back(vec4(quad[0],quad[1],quad[2],quad[3]));
				uvForQuad.push_back(vec2(uvs[0],uvs[1]));
				quad.empty();
				uvs.empty();
			}
			quad.push_back(patch.getVert(i+4*j));
			uvs.push_back(vec2(i/4.0f,j/4.0f)); // u,v as canonical values
		}
	}
	//	loop through and create TriMeshes from each quadrilateral
	int which = 1;
	while (!quadrilaterals.empty()) {
		if (which == 0) {
			quadrilaterals.pop_back();
			uvForQuad.pop_back();
		}
		TriMesh t = getTriMesh(quadrilaterals[quadrilaterals.size()-1], uvForQuad[uvForQuad.size()-1], which);
		// now we pass these TriMeshes into adaptivesubdividepatch
	}
}

//given a patch, perform adaptive subdivision with triangles
TriMesh adaptivesubdividepatch(TriMesh patch, float error) {
//	vec3 curve[3]; // use patch.vertsVec
//	vec2 uvs[3]; // use patch.uvValues
	vec3 splitEdges[3];
	vec3 trianglePoint[3];
	vec3 bezierPoint[3];
	// need to do the following for all 3 sides of the triangle
	// if the bezierPoint is some error away from the triangle midpoint
	//	then we create a new vertex, and split the triangle around that one
	// appropriately
	//	p0,p1 is edge 0
	//	p1,p2 is edge 1
	// 	p2,p0 is edge 2
	
	trianglePoint[0] = 0.5f * (patch.vertsVec[0]+patch.vertsVec[1]);
	trianglePoint[1] = 0.5f * (patch.vertsVec[1]+patch.vertsVec[2]);
	trianglePoint[2] = 0.5f * (patch.vertsVec[2]+patch.vertsVec[0]);
	
	bezierPoint[0] = bezpatchinterp(patch,patch.uvValues[0][0],patch.uvValues[0][1]);
	bezierPoint[1] = bezpatchinterp(patch,patch.uvValues[1][0],patch.uvValues[1][1]);
	bezierPoint[2] = bezpatchinterp(patch,patch.uvValues[2][0],patch.uvValues[2][1]);
	
	for (int i = 0; i < 3; i++) {
		if (error > (bezierPoint[i] - trianglePoint[i]).length2()) {
			splitEdges[i] = NULL;
		} else {
			splitEdges[i] = bezierPoint[i];
		}
	}
	// more stuff coming soon
}