#include "classes.h"

using namespace std;

mat4 Identity4 = mat4(vec4(1, 0, 0, 0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1));


Material::Material() {
	ka = vec3(.1,.1,.1);
	kd = vec3(1,1,1);
	ks = vec3(0,0,0);
	kr = vec3(0,0,0);
	sp = 0;
	ri = 0.0;
	textureIndex = -1;
}

Material::Material(vec3 a, vec3 d, vec3 s, vec3 r, int p) {
	ka = a;
	kd = d;
	ks = s;
	sp = p;
	kr = r;
	textureIndex = -1;
}

Ray::Ray() {
	pos = vec4(0,0,0,0);
	dir = vec4(0,0,0,0);
	refracted = false;
	curRI = 1.0;
	oldRI = 1.0;
}	

Ray::Ray(vec4 a, vec4 b) {
	pos = a;
	dir = b;
	dir.normalize();
	refracted = false;
	curRI = 1.0;
	oldRI = 1.0;
}	

Ray::Ray(vec4 a, vec4 b, float r) {
	pos = a;
	dir = b;
	dir.normalize();
	curRI = r;
	oldRI = 1.0;
}

Ray::Ray(vec4 a, vec4 b, bool t) {
	pos = a;
	dir = b;
	dir.normalize();
	refracted = t;
	curRI = 1.0;
	oldRI = 1.0;
}


Ray::Ray(vec4 a, vec4 b, float rn, float ro, bool t) {
	pos = a;
	dir = b;
	dir.normalize();
	curRI = rn;
	oldRI = ro;
	refracted = t;
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
	//center = tmat*center;
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
	//center = tmat*center;
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
	//center = tmat*center;
	imat = tmat.inverse();
}

mat3 Renderable::dehomogenize(mat4 t) {
	return mat3(
				vec3(tmat[0][0],tmat[0][1],tmat[0][2]),
				vec3(tmat[1][0],tmat[1][1],tmat[1][2]),
				vec3(tmat[2][0],tmat[2][1],tmat[2][2])
				);
}

Texture::Texture() {}

Texture::Texture(const char* fn) {
	txt = FreeImage_Load(FIF_PNG, fn, PNG_DEFAULT);
	width = FreeImage_GetWidth(txt);
	height = FreeImage_GetHeight(txt);
}

vec3 Texture::getColor(float x, float y) {
//TODO: currently, this just gets pixels from a png input, but I want to enable function that generate textures too
	RGBQUAD color;

	FreeImage_GetPixelColor(txt, x, y, &color);
	float r = color.rgbRed / 255.0f;
	float g = color.rgbGreen / 255.0f;
	float b = color.rgbBlue / 255.0f;
	return vec3(r,g,b);
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
//shouldn't ever use  this
vec3 Camera::textureColor (vec4 t) {
	return vec3(-1,-1,-1);
}
Ray Camera::generate_ray (float u, float v) {
	
	vec4 p = (1-u)*((1-v)*(tmat*LL) + v*(tmat*UL)) + (u * ((1-v) * (tmat*LR) + v * (tmat*UR)));

	Ray r = Ray(tmat*pos, p-tmat*pos);
	
	return r;
}


Sphere::Sphere() : Renderable() {
	center = vec3(0,0,0);
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
	vec4 vn, ve, vp;
	vp = imat*hitPoint; // vector from center of sphere to hitPoint
	return color;
}

Triangle::Triangle(vec4 a, vec4 b, vec4 c) : Renderable() {
	v1 = a;
	v2 = b;
	v3 = c;
	center = ((v1+v2+v3)/3).dehomogenize();

}
	
float Triangle::ray_intersect ( Ray r) {
	vec4 raypos = imat*r.pos;
	vec4 raydir = imat*r.dir;
	raydir.normalize();
	float t;
	
	if (fabs(normal() * raydir) == 0) {
		return -1;
	}		
	
	
	vec4 u = v2-v3;
	vec4 v = v1-v3;
	
	vec4 norm = vec4((u.dehomogenize()^v.dehomogenize()).normalize(),0);

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
	return normal();
}

vec4 Triangle::normal() {
	vec4 n = tmat * vec4((v1-v3).dehomogenize()^(v2-v3).dehomogenize(),0);	
	n.normalize();
	
	return n;
}

vec3 Triangle::textureColor(vec4 hitPoint) {
	vec3 color;

	return color;
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
	v = p;
	intensity = i;
}

DLight::DLight(vec4 d, vec3 i) {
	v = d;
	intensity = i;
}

vec4 PLight::lightVector(vec4 origin) {
	return v-origin;
}

vec4 DLight::lightVector(vec4 origin) {
	return -v;
}
