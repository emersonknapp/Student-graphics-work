#include "classes.h"

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
