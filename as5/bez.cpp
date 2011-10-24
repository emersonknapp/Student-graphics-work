//************************//
// Subdivision Code
//************************//

//given a patch, perform adaptive subdivision with triangles
TriMesh adaptivesubdividepatch(TriMesh patch, float error) {
	vec3 curve[3];
	vec3 splitEdges[3];
	vec3 trianglePoint[3];
	vec3 bezierPoint = bezcurveinterp(curve, 0.5f).point;
	// need to do the following for all 3 sides of the triangle
	// if the bezierPoint is *inside* the triangle, then subdivide
	// if it's outside, then we make a new triangle. This will have overlapping
	// triangles for a little bit, but the conflicting triangle should be subdivided
	// appropriately
	//	p0,p1 is edge 0
	//	p1,p2 is edge 1
	// 	p2,p0 is edge 2
	
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