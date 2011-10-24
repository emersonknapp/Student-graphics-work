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



//given a patch, perform uniform subdivision
QuadMesh uniformsubdividepatch(QuadMesh patch, float step) {
	   //compute how many subdivisions there are for this step size
	   float numdiv = int(1.0/step);
	   
	   float stepsize = 1/numdiv;
	   
	   float u,v;
	   QuadMesh newsurface;
	   //for each parametric value of u
	   for (int iu = 0; iu <= numdiv; iu++) {
			   u = iu*stepsize;
			   //for each parametric value of v
			   for (int iv = 0; iv<=numdiv;iv++) {
					v = iv*stepsize;
					//evaluate surface
					//printf("Calculating point (%f, %f)\n", u, v);
					LocalGeo g = bezpatchinterp(patch,u,v);
					//printf("=(%f, %f, %f)\n", g.point[0], g.point[1], g.point[2]);
					newsurface.verts.push_back(g.pos);
					newsurface.norms.push_back(g.dir);
			   }
	   }
	   return newsurface;
}

//given a control patch and (u,v) values, find the surface point and normal
LocalGeo bezpatchinterp(QuadMesh patch, float u, float v) {
	   vec3 vcurve[4];
	   vec3 ucurve[4];
	   vec3 cc[4][4]; //takes the points from patch and groups them appropriately for bezcurveinterp
	   LocalGeo gv, gu;
	   LocalGeo greturn;
	   
	   //build control point for a Bezier curve in v
	   for (int i =0; i<4; i++) {
			   for (int j=0; j<4; j++) {
					   cc[i][j] = patch.verts[i+j*4];
			   }
	   }
	   
	   for (int j=0; j<4; j++) {
			vcurve[j] = bezcurveinterp(cc[j], u).point; 
	   }
	  
	   
	   //build control points for a Bezier curve in u
	   for (int i=0; i<4; i++) {
			   for (int j=0; j<4; j++) {
					   cc[i][j] = patch.verts[i*4+j];
			   }
	   }
	   for (int j=0; j<4; j++) {
			ucurve[j] = bezcurveinterp(cc[j], v).point;
	   } 
	   
	   
	   //evaluate surface and derivative for u and v
	   gv = bezcurveinterp(vcurve, v);
	   gu = bezcurveinterp(ucurve, u);
	   //printf("	 point on v(%f): (%f, %f, %f)\n", v, gv.point[0], gv.point[1], gv.point[2]);
	   //printf("	 point on u(%f): (%f, %f, %f)\n", u, gu.point[0], gu.point[1], gu.point[2]);
	   
	   //take cross product of partials to find normal
	   vec3 n = gv.dir ^ gu.dir;
	   n = n.normalize();
	   greturn.point = gu.point;
	   greturn.dir = n;
	   return greturn;
	   
}

LocalGeo bezcurveinterp(vec3 curve[], float u) {
	   vec3 A,B,C,D,E,p;
	   //first, split each of the three segments to form two new ones AB and BC
	   A = curve[0] * (1.0-u) + curve[1]*u;
	   B = curve[1] * (1.0-u) + curve[2]*u;
	   C = curve[2] * (1.0-u) + curve[3]*u;
	   //now, split AB and BC to form new segment DE
	   D = A * (1.0-u) + B * u;
	   E = B * (1.0-u) + C * u;
	   
	   //finally, pick the right point on DE, this is the point on the curve
	   p = D*(1.0-u)+E*u;
	   //compute the derivate also
	   vec3 dPdu = 3*(E-D);
	   LocalGeo greturn;
	   greturn.point = p;
	   greturn.dir = dPdu;
	   return greturn;
}