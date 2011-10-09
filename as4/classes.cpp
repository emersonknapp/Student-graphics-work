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
	dir.normalize();
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

void Renderable::translate (float x, float y, float z) { // generates translation matrix and updates tmat
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

void Renderable::scale(float xScale, float yScale, float zScale) { // generates scale matrix and updates tmat
	if (xScale == 0) xScale = 1.0f;
	if (yScale == 0) yScale = 1.0f;
	if (zScale == 0) zScale = 1.0f;
	
	mat4 s = mat4(
				vec4(1.0f/xScale,0,0,0),
				vec4(0,1.0f/yScale,0,0),
				vec4(0,0,1.0f/zScale,0),
				vec4(0,0,0,1)
				);
	tmat = tmat * s;
	imat = tmat.inverse();
}

mat3 Renderable::dehomogenize(mat4 t) {
	return mat3(
				vec3(tmat[0][0],tmat[0][1],tmat[0][2]),
				vec3(tmat[1][0],tmat[1][1],tmat[1][2]),
				vec3(tmat[2][0],tmat[2][1],tmat[2][2])
				);
}

vec3 Renderable::dehomogenize(vec4 v) {
	return vec3(v[0],v[1],v[2]);
}



Camera::Camera() {
	pos = vec4(0,0,1,1);
	UL = vec4(-1, 1, 0, 1);
	UR = vec4(1,1,0,1);
	LL = vec4(-1,-1, 0,1);
	LR = vec4(1,-1,0,1);

}
//

float Camera::ray_intersect (Ray r) {
	return -1;
}
vec4 Camera::normal (vec4 v) {
	return vec4(0,0,1,0);
}

Ray Camera::generate_ray (float u, float v) {
	
	vec4 p = (1-u)*((1-v)*(tmat*LL) + v*(tmat*UL)) + (u * ((1-v) * (tmat*LR) + v * (tmat*UR)));
	//printf("Drawing point %f %f = %f %f %f \n", u, v, p[0], p[1], p[2]);

	//vec4 p = u*(v*UR + (1-v)*LR)+(1-u)*(v*UL + (1-v)*LL);
	Ray r = Ray(tmat*pos, p-tmat*pos);
	//Ray r = Ray(pos, p-pos);
	
	return r;
}


Sphere::Sphere(float a) : Renderable() {
	radius = a;
	pos = vec4(0,0,0,1);
}

float Sphere::ray_intersect (Ray r) {
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	vec3 P0 = vec3(raypos[0], raypos[1], raypos[2]);
	vec3 V = vec3(raydir[0], raydir[1], raydir[2]);
	vec3 O = vec3(0,0,0);
	
	float a = 1;
	float b = 2*V * (P0-O);
	float c = (P0-O).length2() - radius*radius;
	
	float discrim = b*b - 4*a*c;
	if (discrim >= 0) {
		float x1 = ((-1*b) - sqrt(discrim))/2;
		float x2 = ((-1*b) + sqrt(discrim))/2;
		float t = min(x1,x2);
		
		vec4 intersection = raypos + t * raydir;
        if (r.dir[2] != 0) t = (tmat*intersection - r.pos)[2] / r.dir[2];
		else if (r.dir[1] != 0) t = (tmat*intersection - r.pos)[1] / r.dir[1];
		else if (r.dir[0] != 0) t = (tmat*intersection - r.pos)[0] / r.dir[0];
		return t;
	} else 
		return -1;
	
	//cout << r.dir << endl;
	/*
	vec4 raydir = r.dir;
	vec4 raypos = r.pos;
	vec4 tpos = tmat*pos;

	vec3 P0 = vec3(raypos[0], raypos[1], raypos[2]);
	vec3 V = vec3(raydir[0], raydir[1], raydir[2]);
	vec3 O = vec3(tpos[0], tpos[1], tpos[2]);
	
	vec3 L = O-P0;
	float tca = L * V;
	if (tca < 0) return -1;
	
	float r2 = radius*radius;
	float d2 = L*L - tca*tca;
	if (d2 > r2) return -1;
	
	cout << raydir << endl;
	cout << raypos << endl;
	cout << tpos << endl;
	
	
	
	float thc = sqrt(r2 - d2);
	cout << tca  << " " << thc << endl;
	float t = min(tca-thc, tca+thc);
	
	vec4 intersection = (raypos + (t*raydir));
	//if (intersection[2] > 1.95)
	cout << imat*intersection << endl << endl;
	
	return t;
	*/
	/*
	vec3 e = vec3(raypos[0], raypos[1], raypos[2]);
	vec3 d = vec3(raydir[0], raydir[1], raydir[2]);
	vec3 incident = e;
	float d_square = d*d;
	float d_dot_incident = d*incident;
	float discrim = (d_dot_incident*d_dot_incident) - d_square * ((incident*incident)-(radius*radius));
	*/
	//cout << "Ray info" << endl;

	//cout <<r.pos << endl;
	/*
	vec3 A = vec3(raypos[0], raypos[1], raypos[2]);
	vec3 C = vec3(0,0,0);
	vec3 D = vec3(raydir[0], raydir[1], raydir[2]);
	
 
	double b = 2*D*(A-C);
	double c = (A-C)*(A-C) - radius*radius; 
	double discrim = b*b - 4*c;
	*/
	
	/*
	if (discrim >= 0) {
		
		float sq_discrim = sqrt(discrim)/d_square;
		float neg_b = -d_dot_incident/d_square;
		float x1 = neg_b - sq_discrim;
		float x2 = neg_b + sq_discrim;
		
		
		
		double x1 = (-b + sqrt(discrim)) / 2;
		double x2 = (-b - sqrt(discrim)) / 2;
		double tempT = min(x1,x2);
		
		
		//vec4 intersection = raypos + tempT * raydir;
		
		vec3 intersection = vec3(0,0,0);
		if (x1 > 6.95) {
			//cout << e << endl;
			//cout << d << endl;
			cout << discrim << endl;
			cout << x1 << " " << x2 << endl;
	
		}
		
        //t = (tmat*intersection - r.pos)[2] / r.dir[2];
		t = intersection[2];
		

		return t>=1;
	}
	*/
	
	//return false;
}

vec4 Sphere::normal(vec4 surface) {
	vec4 place = imat*surface;
	vec4 norm = place - pos;
	norm = tmat*norm;
	norm.normalize();
	return norm;
}

Triangle::Triangle(vec4 a, vec4 b, vec4 c) : Renderable() {
	v1 = a;
	v2 = b;
	v3 = c;
}
	
float Triangle::ray_intersect ( Ray r) {
	// res : Beta | gamma | t
	float t;
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
		}
		return t;
	} else {
		return -1;
	}
}

vec4 Triangle::normal(vec4 surface) {
	return vec4(0,0,0,0);
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


