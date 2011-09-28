//
// Basic class definition for anything we're going to render or apply transformations to
// Call the map/rotate/scale transformation methods in the order you want to apply the transformations.
// The resulting transformation matrix will be stored in tmat, which has transpose() and inverse() methods
// for the necessary operations
//
class Renderable {
public:
	mat4 tmat;
	// constructors
	Renderable () {
		tmat = mat4(
				vec4(0,0,0,0),
				vec4(0,0,0,0),
				vec4(0,0,0,0),
				vec4(1,0,0,0)
				);
	}
	//this is redundant, but it's a nice abstraction (so we don't need an extra map)
	Renderable (int x, int y, int z) {
		tmat = mat4(
				vec4(x,0,0,0),
				vec4(y,0,0,0),
				vec4(z,0,0,0),
				vec4(1,0,0,0)
				);	
	}
	// methods
	void map (int xMap, int yMap, int zMap) { // generates map matrix and updates tmat
		mat4 m = mat4(
					vec4(1,0,0,xMap),
			 		vec4(0,1,0,yMap),
			 		vec4(0,0,1,zMap),
			 		vec4(0,0,0,1)
					);
		tmat = tmat * m;
	}
	void rotate(int angle, vec3 u) { // generates rotation matrix and updates tmat. rotates angle around vector u
		u.normalize();
		float factor = sin(angle/2.0f);
		vec4 q = vec4(cos(angle/2.0f),u[0]*factor,u[1]*factor,u[2]*factor);
		int w = q[0];
		int x = q[1];
		int y = q[2];
		int z = q[3];
		int ww = pow(q[0],2);
		int xx = pow(q[1],2);
		int yy = pow(q[2],2);
		int zz = pow(q[3],2);
		mat4 r = mat4(
					vec4(ww+xx-yy-zz,2*x*y+2*w*z,2*x*z-2*w*y,0),
					vec4(2*x*y-2*w*z,ww-xx+yy-zz,2*y*z+2*w*x,0),
					vec4(2*x*z+2*w*y,2*y*z-2*w*z,ww-xx-yy+zz,0),
					vec4(0,0,0,ww+xx+yy+zz)
				);
		tmat = tmat * r;
	}
	void scale(int xScale, int yScale, int zScale) { // generates scale matrix and updates tmat
		mat4 s = mat4(
					vec4(xScale,0,0,0),
					vec4(0,yScale,0,0),
					vec4(0,0,zScale,0),
					vec4(0,0,0,1)
					);
		tmat = tmat * s;
	}
	
	vec4 ray_intersect (); // returns a vec3 of the appropriate colors for r,g,b

};

//we put all the objects inside a Scene, which is a cube
// -100 ≤ x ≤ 100
// -100 ≤ y ≤ 100
// -100 ≤ z ≤ 100, so the center is at (0,0,0)
//

class Sphere : public Renderable {
// inherits tmat from Renderable
public:
	int radius;
	
	Sphere (int r) {
		radius = r;
	}
	// we can scale the sphere in order to make an ellipsoid
};

class Triangle : public Renderable {
public:
	//vertices
	int v1, v2, v3;
	Triangle (int a, int b, int c) {
		v1 = a;
		v2 = b;
		v3 = c;
	}
};