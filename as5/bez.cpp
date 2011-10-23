//************************//
// Subdivision Code
//************************//

//given a patch, perform uniform subdivision

Surface subdividepatch(Surface patch, float step) {
       //compute how many subdivisions there are for this step size
       float numdiv = int(1.0/step);
       
       float stepsize = 1/numdiv;
       
       float u,v;
       Surface newsurface;
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
                       newsurface.points.push_back(g);
               }
       }
       return newsurface;
}

//given a control patch and (u,v) values, find the surface point and normal
LocalGeo bezpatchinterp(Surface patch, float u, float v) {
       vec3 vcurve[4];
       vec3 ucurve[4];
       vec3 cc[4][4]; //takes the points from patch and groups them appropriately for bezcurveinterp
       LocalGeo gv, gu;
       LocalGeo greturn;
       
       //build control point for a Bezier curve in v
       for (int i =0; i<4; i++) {
               for (int j=0; j<4; j++) {
                       cc[i][j] = patch.points[i+j*4].point;
               }
       }
       
       for (int j=0; j<4; j++) {
       		vcurve[j] = bezcurveinterp(cc[j], u).point;	
       }
      
       
       //build control points for a Bezier curve in u
       for (int i=0; i<4; i++) {
               for (int j=0; j<4; j++) {
                       cc[i][j] = patch.points[i*4+j].point;
               }
       }
       for (int j=0; j<4; j++) {
       		ucurve[j] = bezcurveinterp(cc[j], v).point;
       } 
       
       
       //evaluate surface and derivative for u and v
       gv = bezcurveinterp(vcurve, v);
       gu = bezcurveinterp(ucurve, u);
       //printf("    point on v(%f): (%f, %f, %f)\n", v, gv.point[0], gv.point[1], gv.point[2]);
       //printf("    point on u(%f): (%f, %f, %f)\n", u, gu.point[0], gu.point[1], gu.point[2]);
       
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