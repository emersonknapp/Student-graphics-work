#include "classes.h"




using namespace std;



Material::Material() {
	ka = vec3(0,0,0);
	kd = vec3(0,0,0);
	ks = vec3(0,0,0);
	kr = 0;
	sp = 0;
}

Material::Material(vec3 a, vec3 d, vec3 s, float r, int p) {
	ka = a;
	kd = d;
	ks = s;
	sp = p;
	kr = r;
}

Ray::Ray() {
	pos = vec4(0,0,0,0);
	dir = vec4(0,0,0,0);
}	

Ray::Ray(vec4 a, vec4 b) {
	pos = a;
	dir = b;
}
	


Renderable::Renderable () {
	tmat = mat4(
			vec4(1,0,0,0),
			vec4(0,1,0,0),
			vec4(0,0,1,0),
			vec4(0,0,0,1)
			);
}

void Renderable::translate (vec3 t) {
	translate(t[0], t[1], t[2]);
}

void Renderable::translate (int x, int y, int z) { // generates translation matrix and updates tmat
	mat4 m = mat4(
				vec4(1,0,0,x),
		 		vec4(0,1,0,y),
		 		vec4(0,0,1,z),
		 		vec4(0,0,0,1)
				);
	tmat = tmat * m;
	
	imat = tmat.inverse();
	
}
void Renderable::rotate(int angle, vec3 u) { // generates rotation matrix and updates tmat. rotates angle around vector u
	if (angle != 0 && u != vec3(0,0,0)) {
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
}

void Renderable::scale (vec3 s) {
	scale(s[0], s[1], s[2]);
}

void Renderable::scale(int xScale, int yScale, int zScale) { // generates scale matrix and updates tmat
	mat4 s = mat4(
				vec4(xScale,0,0,0),
				vec4(0,yScale,0,0),
				vec4(0,0,zScale,0),
				vec4(0,0,0,1)
				);
	tmat = tmat * s;
	imat = tmat.inverse();
}


Camera::Camera() {
	pos = vec4(0,0,-1.0f,1.0f);
	up = vec4(0,1.0f,0,1.0f);
	viewer = vec4(0,0,1.0f,1.0f);
}
//

bool Camera::ray_intersect (Ray &r, float &t, vec4 &normal) {
	return false;
}

Ray Camera::generate_ray (float x, float y, Viewport v) {
	// |x|,|y| should be 0 <= j <= 1
	vec4 tmp = vec4(x*v.w/2.0f,y*v.h/2.0f,0,1);
	vec4 dir = (tmp - pos);
	return Ray(pos, dir);
}


Sphere::Sphere(int a) : Renderable() {
	radius = a;
	base = vec4(0,0,0,1);
}

bool Sphere::ray_intersect ( Ray& r, float &t, vec4 &normal) {
	vec4 pos = tmat * base;
	vec4 tmpDir = r.dir;
	tmpDir.normalize();
	float a = tmpDir.length2();
	float b = 2*tmpDir * (r.pos-pos);
	float c = (r.pos - pos) * (r.pos - pos) - pow(radius,2.0f);
//	cout << "a: " << a << " b: " << b << " c: " << c << endl;
	float tmp1 = (-b + sqrt(pow(b,2)-4*a*c)) / (2*a);
	float tmp2 = (-b - sqrt(pow(b,2)-4*a*c)) / (2*a);
	vec4 x = r.pos + tmp1*r.dir - pos;
	vec4 y = r.pos + tmp2*r.dir - pos;
//	cout << "tmp1: " << tmp1 << " tmp2: " << tmp2 << endl;
//	cout << x.length2() - pow(radius,2.0f) << endl;
//	cout << y.length2() - pow(radius,2.0f) << endl;
	//TODO: currently it's all falling into this false case, so there's some error with computing the a,b,c (a is too large?)
	if (pow(b,2)-4*a*c < 0 ) {
		return false;
	} else {
		// this t determines intersection point A + t*D = r.pos + t * r.dir
		float tmp1 = max((-b + sqrt(pow(b,2)-4*a*c)) / (2*a), 0.0f);
		float tmp2 = max((-b - sqrt(pow(b,2)-4*a*c)) / (2*a), 0.0f);
//		cout << "tmp1: " << tmp1 << " tmp2: " << tmp2 << endl;
		float tmp = min(tmp1,tmp2);
//		cout << "tmp is: "<< tmp << endl;
		if (tmp == 0.0f || tmp == INT_MAX || tmp <= 1.0f) {
			return false;
		}
		else if (tmp < t) {
			t = tmp;
			vec4 intersection = r.pos + t * r.dir; // this is a point on the sphere
			// if we have normal n on the sphere, then if it's transformed, we use (M^-1)^T * n as the normal
			// intersection - pos is the normal on the sphere
			normal = tmat.inverse().transpose() * (intersection - pos);
			normal.normalize();
			return true;
		}
		return false;
	}
}

Triangle::Triangle(vec4 a, vec4 b, vec4 c) : Renderable() {
	v1 = a;
	v2 = b;
	v3 = c;
}
	
bool Triangle::ray_intersect ( Ray &r, float &t, vec4 &normal ) {
	// res : Beta | gamma | t
	cout << "Triangle ray intersect check" << endl;
	vec3 res = mat4(
					vec4((v2-v1)[0],(v3-v1)[0],-r.dir[0],0),
					vec4((v2-v1)[1],(v3-v1)[1],-r.dir[1],0),
					vec4((v2-v1)[2],(v3-v1)[3],-r.dir[2],0),
					vec4(0,0,0,0)
					).inverse() * vec4(r.pos) - v1;
	cout << "Checking parameters are within range..." << endl;
	if (res[0] > 0 && res[1] > 0 && res[0]+res[1] <= 1 && res[2] < t) {
		int tmp = res[2];
		if (tmp < t) {
			t = tmp;
			vec4 intersection = r.pos + t * r.dir; // this is a point on the triangle
			normal = tmat.inverse().transpose() * ((v1-t) ^ (v3-t)); // the second part is the cross product of two vectors that define the triangle from point t
			normal.normalize();
		}
		return true;
	} else {
		return false;
	}
}


Scene::Scene() {
	activeCamera=0;
}

Scene::~Scene() {
	for (int i=0; i<renderables.size(); i++) delete renderables[i];
	for (int i=0; i<plights.size(); i++) delete plights[i];
	for (int i=0; i<dlights.size(); i++) delete dlights[i];
	for (int i=0; i<cameras.size(); i++) delete cameras[i];
}

Viewport::Viewport () {
	w = 0;
	h = 0;
}
Viewport::Viewport (int width, int height) {
	w = width;
	h = height;
}

PLight::PLight(vec4 p, vec3 i) {
	pos = p;
	intensity = i;
}

DLight::DLight(vec4 d, vec3 i) {
	dir = d;
	intensity = i;
}


