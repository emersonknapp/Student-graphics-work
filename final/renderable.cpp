#include "classes.h"
#include "pc.h"
using namespace std;

mat4 Identity4 = mat4(vec4(1, 0, 0, 0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1));

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
	center = tmat*center;
	imat = tmat.inverse();
	
}

void Renderable::rotate(vec3 r) {
	rotate(r[0], r[1], r[2]);
}

void Renderable::rotate(float x, float y, float z) { // generates rotation matrix and updates tmat. euler angles, yo	
	
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
	center = tmat*center;
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
	center = tmat*center;
	imat = tmat.inverse();
}

mat3 Renderable::dehomogenize(mat4 t) {
	return mat3(
				vec3(tmat[0][0],tmat[0][1],tmat[0][2]),
				vec3(tmat[1][0],tmat[1][1],tmat[1][2]),
				vec3(tmat[2][0],tmat[2][1],tmat[2][2])
				);
}

AABB* Renderable::makeAABB() {
	aabb = new AABB();
	return aabb;
}

Sphere::Sphere() : Renderable() {
	center = vec3(0,0,0);
}

float Sphere::rayIntersect (Ray r) {
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
		if ((t*raydir) * (raydir) < 0) { //if the ray folds back on itself. We always want ot move *forward*, so we check if the dot product is negative
			t = max(x1,x2);
		}
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

vec3 Sphere::textureColor(vec4 hitPoint) {
	vec3 color;
	float u,v;
	vec4 vn, ve, vp;
	vp = imat*hitPoint; // vector from center of sphere to hitPoint
	vn = vec4(0,0,1,0); // I *think*, but I'm not sure that I want to treat the 'north pole' as (0,0,1) on the sphere
	ve = vec4(1,0,0,0); // same with this point on the equator
	//latitude
	float phi = acos(-vn * vp);
	v = phi / 3.1415;	
	//longitude
	float theta = acos( (vp * ve) / sin(phi) ) / (2 * 3.1415);
	vec4 tmp = vec4((vn.dehomogenize() ^ ve.dehomogenize()),0);
	if (tmp * vp > 0) u = theta;
	else u = 1.0-theta;
	color = material.texture.getColor(u,v); // v is latitude (y) , u is longitude (x)
	return color;
}

vec4 Sphere::randomSurfacePoint() {
	vec4 point = randomSpherePoint();
	return tmat * point;
}

AABB* Sphere::makeAABB() {
	aabb = new AABB();
	aabb->mins = vec3(-1,-1,-1);
	aabb->maxes = vec3(1,1,1);
	
	//create box vertices and tranform by tmat
	vec4 box[8];
	int bit;
	for (int i=0; i<8; i++) {
		for (int j=0; j<3; j++) {
			bit = (i>>j)%2;
			box[i][j] = (1-bit)*aabb->mins[j] + bit*aabb->maxes[j];
		}
		box[i][3] = 1;
		box[i] = tmat*box[i];
	}
	//reconstruct aabb
	aabb->mins = box[0];
	aabb->maxes = box[0];
	for (int i=0; i<8; i++) {
		for (int j=0; j<3; j++) {
			aabb->mins[j] = min(aabb->mins[j], box[i][j]);
			aabb->maxes[j] = max(aabb->maxes[j], box[i][j]);
		}
	}
	
	
	return aabb;
}

Triangle::Triangle(vec4 a, vec4 b, vec4 c) : Renderable() {
	v1 = a;
	v2 = b;
	v3 = c;
	center = ((v1+v2+v3)/3).dehomogenize();
}
	
	
Triangle::Triangle(vec4 a, vec4 b, vec4 c, vec3 g, vec3 h, vec3 i, vec4 m, vec4 n, vec4 o) : Renderable() {
	v1 = a;
	v2 = b;
	v3 = c;
	vt1 = g;
	vt2 = h;
	vt3 = i;
	vn1 = m;
	vn2 = n;
	vn3 = o;
	center = ((v1+v2+v3)/3).dehomogenize();
}
	
float Triangle::rayIntersect ( Ray r) {
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	raydir.normalize();
	float t;
	
	vec4 u = v2-v3;
	vec4 v = v1-v3;
	
	vec4 norm = vec4((u.dehomogenize()^v.dehomogenize()).normalize(),0);

	if (fabs(norm * raydir) == 0) {
		return -1;
	}		
	
	if (norm == vec4(0,0,0,0)) {
		return -1;
	}

	vec4 w0 = raypos - v3;

	float a = -(norm * w0);
	float b = (norm * raydir);

	if (fabs(b) < WIGGLE) { //ray is parallel to triangle
		return -1;
	}

	// get ray intersect point
	
	t = a / b; // t is the parametric intersection of ray with triangle
	if (t < 0.0) return -1; //ray pointing away from triangle

	// check if the ray intersects within the triangle (isntead of just the triangle plane)
	
	float uu, uv, vv, wu, wv, D;

	uu = u*u;
	uv = u*v;
	vv = v*v;
	vec4 w = (raypos + t*raydir - v3);
	wu = w*u;
	wv = w*v;
	D = uv * uv - uu * vv;
	float s,f;

	s = (uv * wv - vv * wu) / D;
	if (s < 0.0 or s > 1.0) return -1; // outside
	
	f = (uv * wu - uu * wv) / D;
	if (f < 0.0 or (s+f) > 1.0) return -1; // outside

	return t;
}

vec4 Triangle::normal(vec4 surface) {

	vec4 n = tmat * vec4((v1-v3).dehomogenize()^(v2-v3).dehomogenize(),0);	

	if (vn1 != vec4(0,0,0,0)) {
		vec3 nn = n.dehomogenize();
		vec3 bary = barycentric(v1,v2,v3,surface);
		norm = bary[0] * vn1 + bary[1] * vn2 + bary[2] * vn3;
		norm.normalize();
		return vec4(norm,0); 

	} else {
		n.normalize();
		
		return n;
	}
}

vec3 Triangle::textureColor(vec4 hitPoint) {
	// Triangle has texture vertices between 0.0 and 1.0 (canonical coordinates of the texture map we're going to use)
	// we specify vertices "vt .3 .2 0", etc, and then reference which texture vertices we're using w/ this triangle
	// u, v, w (horizontal, vertical, depth)...not sure if we'll go depth
	vec3 color;
	vec3 bary = barycentric(v1,v2,v3,hitPoint);
	vec3 point = bary[0] * vt1 + bary[1] * vt2 + bary[2] * vt3;
	color = material.texture.getColor(point[0],point[1]);

	return color;
}

vec4 Triangle::randomSurfacePoint() {
	float a = rand01();
	float b = rand01();
	if (a+b > 1) {
		a = 1.0f-a;
		b = 1.0f-b;
	}
	float c = 1-a-b;
	return v1*a + v2*b + v3*c;
}

AABB* Triangle::makeAABB() {
	//For triangle, just compute bounding box around transformed vertices, easy.
	vec4 tv1 = tmat*v1;
	vec4 tv2 = tmat*v2;
	vec4 tv3 = tmat*v3;
	aabb = new AABB();
	for (int i=0; i<3; i++) {
		aabb->mins[i] = min(tv1[i], min(tv2[i], tv3[i]));
		aabb->maxes[i] = max(tv1[i], max(tv2[i], tv3[i]));
	}
	
	return aabb;
}

Camera::Camera() {
	pos = vec4(0,0,0,1);
	UL = vec4(-1, 1, -3, 1);
	UR = vec4(1,1,-3,1);
	LL = vec4(-1,-1, -3,1);
	LR = vec4(1,-1,-3,1);
}
//shouldn't ever use this
vec4 Camera::randomSurfacePoint() {
	return vec4(-1,-1,-1,-1);
}

float Camera::rayIntersect (Ray r) {
	return -1;
}
vec4 Camera::normal (vec4 v) {
	return vec4(0,0,1,0);
}
//shouldn't ever use  this
vec3 Camera::textureColor (vec4 t) {
	return vec3(-1,-1,-1);
}
Ray Camera::generate_ray (float u, float v) {
	vec4 p = (1-u)*((1-v)*(tmat*LL) + v*(tmat*UL)) + (u * ((1-v) * (tmat*LR) + v * (tmat*UR)));
	Ray r = Ray(tmat*pos, p-tmat*pos);
	
	return r;
}

