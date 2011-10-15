#include "classes.h"

using namespace std;

mat4 Identity4 = mat4(vec4(1, 0, 0, 0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1));


Material::Material() {
	ka = vec3(0,0,0);
	kd = vec3(0,0,0);
	ks = vec3(0,0,0);
	kr = vec3(0,0,0);
	sp = 0;
}

Material::Material(vec3 a, vec3 d, vec3 s, vec3 r, int p) {
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
	rotmat = Identity4;
	scalemat = Identity4;
	transmat = Identity4;
}

void Renderable::translate (vec3 t) {
	translate(t[0], t[1], t[2]);
}

void Renderable::translate (float x, float y, float z) { // generates translation matrix and updates tmat
	transmat = mat4(
				vec4(1,0,0,x),
		 		vec4(0,1,0,y),
		 		vec4(0,0,1,z),
		 		vec4(0,0,0,1)
				);

	tmat = transmat * rotmat * scalemat;
	
	imat = tmat.inverse();
	
}

void Renderable::rotate(vec3 r) {
	rotate(r[0], r[1], r[2]);
}

void Renderable::rotate(float x, float y, float z) { // generates rotation matrix and updates tmat. rotates angle around vector u	
	
	mat3 rx = mat3(
				vec3(1, 0, 0),
				vec3(0, cos(x), -sin(x)),
				vec3(0, sin(x), cos(x))
			  );
	mat3 ry = mat3(
				vec3(cos(y), 0, sin(y)),
				vec3(0, 1, 0),
				vec3(-sin(y), 0, cos(y))
			  );
	mat3 rz = mat3(
				vec3(cos(z), -sin(z), 0),
				vec3(sin(z), cos(z), 0),
				vec3(0, 0, 1)
			  );
	mat3 rmat = ry * rz * rx;
	rotmat = mat4(
					vec4(rmat[0], 0),
					vec4(rmat[1], 0),
					vec4(rmat[2], 0),
					vec4(0,0,0,1)
				 );

	tmat = transmat * rotmat * scalemat;
	imat = tmat.inverse();
	
}

void Renderable::scale (vec3 s) {
	scale(s[0], s[1], s[2]);
}

void Renderable::scale(float xScale, float yScale, float zScale) { // generates scale matrix and updates tmat
	if (xScale == 0) xScale = 1.0f;
	if (yScale == 0) yScale = 1.0f;
	if (zScale == 0) zScale = 1.0f;
	
	scalemat = mat4(
				vec4(xScale,0,0,0),
				vec4(0,yScale,0,0),
				vec4(0,0,zScale,0),
				vec4(0,0,0,1.0f)
				);
	tmat = transmat * rotmat * scalemat;
	imat = tmat.inverse();
}

mat3 Renderable::dehomogenize(mat4 t) {
	return mat3(
				vec3(tmat[0][0],tmat[0][1],tmat[0][2]),
				vec3(tmat[1][0],tmat[1][1],tmat[1][2]),
				vec3(tmat[2][0],tmat[2][1],tmat[2][2])
				);
}



Camera::Camera() {
	pos = vec4(0,0,0,1);
	UL = vec4(-1, 1, -3, 1);
	UR = vec4(1,1,-3,1);
	LL = vec4(-1,-1, -3,1);
	LR = vec4(1,-1,-3,1);

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

	Ray r = Ray(tmat*pos, p-tmat*pos);
	
	return r;
}


Sphere::Sphere() : Renderable() {
	
}

float Sphere::ray_intersect (Ray r) {
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	raydir.normalize();
	vec3 rayO = raypos.dehomogenize();
	vec3 rayD = raydir.dehomogenize();
	
	float b = 2*(rayD * rayO);
	float c = rayO.length2()-1;
	
	float discrim = b*b - 4*c;
	
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
}

vec4 Sphere::normal(vec4 surface) {
	vec3 hit = (imat*surface).dehomogenize();
	vec4 norm = imat.transpose()*vec4(hit,0);
	norm.normalize();
	
	return norm;
}

Triangle::Triangle(vec4 a, vec4 b, vec4 c) : Renderable() {
	v1 = a;
	v2 = b;
	v3 = c;

}
	
float Triangle::ray_intersect ( Ray r) {
	/*
	// res : Beta | gamma | t
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	raydir.normalize();
	
	vec4 u = v2-v3;
	vec4 v = v1-v3;
	vec4 n = normal();
	float t, a, b;
	vec4 w0 = raypos - v3;
	vec4 hitPoint;
	
	if(n==vec4(0,0,0,0)) {
		return -1;
	}
	
	a = -(n*w0);
	b = n*raydir;
	if(fabs(b) < .001) {
		return -1;
	}
	t = a/b;
	if (t<0.0)
		return -1;
		
	hitPoint = raypos + t*raydir;
	
	float uu, uv, vv, wu, wv, D;
	uu = u*u;
	uv = u*v;
	vv = v*v;
	vec4 w = hitPoint - v3;
	wu = w*u;
	wv = w*v;
	D = uv*uv - uu*vv;
	
	float s, q;
	s = (uv*wv - vv*wu)/ D;
	if (s<0.0 || s>1.0)
		return -1;
	q = (uv * wu - uu*wv) / D;
	if (q < 0.0 || q+s > 1.0)
		return -1;
		
	return t;
	*/
	
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	raydir.normalize();
	float t;
	
	if (normal() * raydir == 0) {
		return -1;
	}		
	
	
	vec4 a = v2-v3;
	vec4 b = v1-v3;
	vec3 res = mat3(
					vec3(a[0],b[0],-raydir[0]),
					vec3(a[1],b[1],-raydir[1]),
					vec3(a[2],b[2],-raydir[2])
					).inverse() * (raypos - v3).dehomogenize();
	if (res[0] > 0 && res[1] > 0 && res[0]+res[1] < 1) {
		t = res[2];
		vec4 intersection = raypos + t * raydir; // this is a point on the triangle
		if (r.dir[2] != 0) t = (tmat*intersection - r.pos)[2] / r.dir[2];
		else if (r.dir[1] != 0) t = (tmat*intersection - r.pos)[1] / r.dir[1];
		else if (r.dir[0] != 0) t = (tmat*intersection - r.pos)[0] / r.dir[0];
		return t;
	} else {
		return -1;
	}
	
	
	
}

vec4 Triangle::normal(vec4 surface) {
	
	vec4 n = tmat * vec4((v1-v3).dehomogenize()^(v2-v3).dehomogenize(),0);	
	n.normalize();
	
	return n;
}

vec4 Triangle::normal() {
	vec4 n = tmat * vec4((v1-v3).dehomogenize()^(v2-v3).dehomogenize(),0);	
	n.normalize();
	
	return n;
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
