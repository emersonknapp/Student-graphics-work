#ifndef _CLASSES_H
#define _CLASSES_H


//
// Basic class definition for anything we're going to render or apply transformations to
// Call the map/rotate/scale transformation methods in the order you want to apply the transformations.
// The resulting transformation matrix will be stored in tmat, which has transpose() and inverse() methods
// for the necessary operations
//
class Material {
public:
	Material() {
		ka = vec3(0,0,0);
		kd = vec3(0,0,0);
		ks = vec3(0,0,0);
		kr = 0;
		sp = 0;
	}
	Material(vec3 a, vec3 d, vec3 s, float r, int p) {
		ka = a;
		kd = d;
		ks = s;
		sp = p;
		kr = r;
	}
	vec3 ka; //ambient
	vec3 kd; //diffuse
	vec3 ks; //specular
	float kr; //reflection coefficient
	int sp; //specular power
};

class Ray {
public:
	vec4 pos;
	vec4 dir;
	float t_min, t_max;
	
	Ray(vec4 a, vec4 b) {
		pos = a;
		dir = b;
	}
	
};

class Renderable {
public:
	mat4 tmat;
	mat4 imat;
	Material material;
	// constructors
	Renderable () {
		tmat = mat4(
				vec4(0,0,0,0),
				vec4(0,0,0,0),
				vec4(0,0,0,0),
				vec4(1,0,0,0)
				);
	}
	//this is redundant, but it's a nice abstraction (so we don't need an extra translation)
	Renderable (int x, int y, int z) {
		tmat = mat4(
				vec4(x,0,0,0),
				vec4(y,0,0,0),
				vec4(z,0,0,0),
				vec4(1,0,0,0)
				);	
	}
	// methods
	void translate (int x, int y, int z) { // generates translation matrix and updates tmat
		mat4 m = mat4(
					vec4(1,0,0,x),
			 		vec4(0,1,0,y),
			 		vec4(0,0,1,z),
			 		vec4(0,0,0,1)
					);
		tmat = tmat * m;
		imat = tmat.inverse();
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
		imat = tmat.inverse();
	}
	void scale(int xScale, int yScale, int zScale) { // generates scale matrix and updates tmat
		mat4 s = mat4(
					vec4(xScale,0,0,0),
					vec4(0,yScale,0,0),
					vec4(0,0,zScale,0),
					vec4(0,0,0,1)
					);
		tmat = tmat * s;
		imat = tmat.inverse();
	}
	
	bool ray_intersect (Ray &r, int &t); // returns whether ray intersects this object, sets t to proper value

};






class Camera : public Renderable {
	// needs to keep track of its position (starts at 0,0,1)
	// keep track of where it's facing? facing -z direction relative to itself
	// transform view plane. Keep track of tmat
public:
	vec4 pos,up,viewer;
	Camera() {
		pos = vec3(0,0,1);
		up = vec3(0,1,0);
		viewer = vec3(0,0,-1);
	}
	
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
	vec4 base;
	
	Sphere (int a) {
		radius = a;
		vec4 base = vec4(0,0,0,1);
	}
	
	bool ray_intersect ( Ray &r, int &t) {
		vec4 pos = tmat * base;
		float a = r.dir.length2();
		float b = 2*r.pos*r.dir + pos * r.dir;
		float c = r.pos.length2() - r.pos*pos + pos.length2() - pos*r.pos - pow(radius,2.0f);
		if (pow(b,2)+4*a*c < 0 ) {
			return false;
		} else {
			t = min((-b + sqrt(pow(b,2)-4*a*c) / (2*a) ), (-b + sqrt(pow(b,2)-4*a*c)) / (2*a));
			return true;
		}
	}
	// we can scale the sphere in order to make an ellipsoid
};

class Triangle : public Renderable {
public:
	//vertices
	vec4 v1, v2, v3;
	Triangle (vec4 a, vec4 b, vec4 c) {
		v1 = a;
		v2 = b;
		v3 = c;
	}
	
	bool ray_intersect ( Ray &r, int &t ) {
		// res : Beta | gamma | t
		vec3 res = mat4(
						vec4((v2-v1)[0],(v3-v1)[0],-r.dir[0],0),
						vec4((v2-v1)[1],(v3-v1)[1],-r.dir[1],0),
						vec4((v2-v1)[2],(v3-v1)[3],-r.dir[2],0),
						vec4(0,0,0,0)
						).inverse() * vec4(r.pos) - v1;
		if (res[0] > 0 && res[1] > 0 && res[0]+res[1] <= 1) {
			t = res[2];
			return true;
		} else {
			return false;
		}
	}
};

#endif