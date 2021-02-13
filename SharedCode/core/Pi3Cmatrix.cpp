#include "Pi3Cmatrix.h"
#include <cmath>
#include <cstring>

Pi3Cmatrix Pi3Cmatrix::operator * (Pi3Cmatrix const &m2) const
{
	//Return matrix result if one of the matrices is identity - saves a lot of mults!
	if (m2.identity) return *this;
	if (identity) return m2;

	//multiply only the bits we need (36 mults instead of 48)...
	Pi3Cmatrix nm;
	nm.matrix[m00] = matrix[m00] * m2.matrix[m00] + matrix[m01] * m2.matrix[m10] + matrix[m02] * m2.matrix[m20]; // + matrix[m03] * m2.matrix[m30];
	nm.matrix[m01] = matrix[m00] * m2.matrix[m01] + matrix[m01] * m2.matrix[m11] + matrix[m02] * m2.matrix[m21]; // + matrix[m03] * m2.matrix[m31];
	nm.matrix[m02] = matrix[m00] * m2.matrix[m02] + matrix[m01] * m2.matrix[m12] + matrix[m02] * m2.matrix[m22]; // + matrix[m03] * m2.matrix[m32];
	//nm.matrix[m03] = matrix[m00] * m2.matrix[m03] + matrix[m01] * m2.matrix[m13] + matrix[m02] * m2.matrix[m23]; // + matrix[m03] * m2.matrix[m33];
	
	nm.matrix[m10] = matrix[m10] * m2.matrix[m00] + matrix[m11] * m2.matrix[m10] + matrix[m12] * m2.matrix[m20]; // + matrix[m13] * m2.matrix[m30];
	nm.matrix[m11] = matrix[m10] * m2.matrix[m01] + matrix[m11] * m2.matrix[m11] + matrix[m12] * m2.matrix[m21]; // + matrix[m13] * m2.matrix[m31];
	nm.matrix[m12] = matrix[m10] * m2.matrix[m02] + matrix[m11] * m2.matrix[m12] + matrix[m12] * m2.matrix[m22]; // + matrix[m13] * m2.matrix[m32];
	//nm.matrix[m13] = matrix[m10] * m2.matrix[m03] + matrix[m11] * m2.matrix[m13] + matrix[m12] * m2.matrix[m23]; // + matrix[m13] * m2.matrix[m33];

	nm.matrix[m20] = matrix[m20] * m2.matrix[m00] + matrix[m21] * m2.matrix[m10] + matrix[m22] * m2.matrix[m20]; // + matrix[m23] * m2.matrix[m30];
	nm.matrix[m21] = matrix[m20] * m2.matrix[m01] + matrix[m21] * m2.matrix[m11] + matrix[m22] * m2.matrix[m21]; // + matrix[m23] * m2.matrix[m31];
	nm.matrix[m22] = matrix[m20] * m2.matrix[m02] + matrix[m21] * m2.matrix[m12] + matrix[m22] * m2.matrix[m22]; // + matrix[m23] * m2.matrix[m32];
	//nm.matrix[m23] = matrix[m20] * m2.matrix[m03] + matrix[m21] * m2.matrix[m13] + matrix[m22] * m2.matrix[m23]; // + matrix[m23] * m2.matrix[m33];

	//nm.matrix[m30] = matrix[m30] * m2.matrix[m00] + matrix[m31] * m2.matrix[m10] + matrix[m32] * m2.matrix[m20]; // + matrix[m33] * m2.matrix[m30];
	//nm.matrix[m31] = matrix[m30] * m2.matrix[m01] + matrix[m31] * m2.matrix[m11] + matrix[m32] * m2.matrix[m21]; // + matrix[m33] * m2.matrix[m31];
	//nm.matrix[m32] = matrix[m30] * m2.matrix[m02] + matrix[m31] * m2.matrix[m12] + matrix[m32] * m2.matrix[m22]; // + matrix[m33] * m2.matrix[m32];
	//nm.matrix[m33] = matrix[m30] * m2.matrix[m03] + matrix[m31] * m2.matrix[m13] + matrix[m32] * m2.matrix[m23]; // + matrix[m33] * m2.matrix[m33];
	
	nm.matrix[m30] = m2.matrix[m00] * matrix[m30] + m2.matrix[m10] * matrix[m31] + m2.matrix[m20] * matrix[m32] + m2.matrix[m30]; //X
	nm.matrix[m31] = m2.matrix[m01] * matrix[m30] + m2.matrix[m11] * matrix[m31] + m2.matrix[m21] * matrix[m32] + m2.matrix[m31]; //Y
	nm.matrix[m32] = m2.matrix[m02] * matrix[m30] + m2.matrix[m12] * matrix[m31] + m2.matrix[m22] * matrix[m32] + m2.matrix[m32]; //Z

	nm.identity = false;
	return nm;
}

void Pi3Cmatrix::SetScale(const float &scale)
{
	matrix[m00] = scale; matrix[m11] = scale; matrix[m22] = scale;
	if (scale!=1.f) identity = false;
}

void Pi3Cmatrix::SetScales(const vec3f &scales)
{
	matrix[m00] = scales.x; matrix[m11] = scales.y; matrix[m22] = scales.z;
	identity = false;
}

void Pi3Cmatrix::SetRotateX(const float &angle)
{
	matrix[m00] = 1.f; matrix[m01] = 0; matrix[m02] = 0; //matrix[m03] = 0;
	matrix[m10] = 0; matrix[m11] = cosf(angle); matrix[m12] = sinf(angle); //matrix[m13] = 0;
	matrix[m20] = 0; matrix[m21] = -sinf(angle); matrix[m22] = cosf(angle); //matrix[m23] = 0.f;
	//matrix[m30] = 0; matrix[m31] = 0; matrix[m32] = 0; matrix[m33] = 1.f;
	identity = false;
}

void Pi3Cmatrix::SetRotateXbit(const float &angle)
{
	matrix[m11] = matrix[m22] = cosf(angle);
	matrix[m21] = -(matrix[m12] = sinf(angle));
	identity = false;
}

void Pi3Cmatrix::SetRotateY(const float &angle)
{
	matrix[m00] = cosf(angle); matrix[m01] = 0; matrix[m02] = sinf(angle); //matrix[m03] = 0;
	matrix[m10] = 0; matrix[m11] = 1.f; matrix[m12] = 0; //matrix[m13] = 0;
	matrix[m20] = -sinf(angle); matrix[m21] = 0; matrix[m22] = cosf(angle); //matrix[m23] = 0.f;
	//matrix[m30] = 0; matrix[m31] = 0; matrix[m32] = 0; matrix[m33] = 1.f;
	identity = false;
}

void Pi3Cmatrix::SetRotateYbit(const float &angle)
{
	matrix[m00] = matrix[m22] = cosf(angle);
	matrix[m20] = -(matrix[m02] = sinf(angle));
	identity = false;
}

void Pi3Cmatrix::SetRotateZ(const float &angle)
{
	matrix[m00] = cosf(angle); matrix[m01] = sinf(angle); matrix[m02] = 0; //matrix[m03] = 0;
	matrix[m10] = -sinf(angle); matrix[m11] = cosf(angle); matrix[m12] = 0; //matrix[m13] = 0;
	matrix[m20] = 0; matrix[m21] = 0; matrix[m22] = 1.f; //matrix[m23] = 0.f;
	//matrix[m30] = 0; matrix[m31] = 0; matrix[m32] = 0; matrix[m33] = 1.f;
	identity = false;
}

void Pi3Cmatrix::SetRotateZbit(const float &angle)
{
	matrix[m00] = matrix[m11] = cosf(angle);
	matrix[m10] = -(matrix[m01] = sinf(angle));
	identity = false;
}

void Pi3Cmatrix::SetPerspective(const int width, const int height, const float pspmul, const float znear, const float zfar)
{
	//GLint vp[4];
	//glGetIntegerv(GL_VIEWPORT, vp);
	float aspectRatio = (float)width / (float)height; // (float)vp[2] / (float)vp[3];  //width / height
	float fov = height / (pspmul*2.f);  //(float)vp[3] / (pspmul*2.f);
	float halfHeight = znear * fov;
	float halfWidth = halfHeight * aspectRatio;
	float zfn = 1.f / (zfar - znear);

	//equivalent setup to glFrustum ...
	memset(&matrix, 0, 60);					//reset matrix with 0's
	matrix[m00] = znear / halfWidth;
	matrix[m11] = znear / halfHeight;
	matrix[m22] = -(zfar + znear) * zfn;
	matrix[m23] = -1.f;
	matrix[m32] = -2.f * zfar * znear * zfn;
	identity = false;
}

void Pi3Cmatrix::SetOrtho(const float left, const float right, const float top, const float bottom, const float znear, const float zfar, const float invert)
{
	//left,right,top,bottom need to be float - esp. for zoom function
	float rl = right - left; 
	float tb = top - bottom;
	float w = invert * (2.f / rl);
	float h = invert * (2.f / tb);
	float zn = -2.f / (zfar - znear);
	matrix[m00] = w; matrix[m01] = 0; matrix[m02] = 0; matrix[m03] = 0;
	matrix[m10] = 0; matrix[m11] = h; matrix[m12] = 0; matrix[m13] = 0;
	matrix[m20] = 0; matrix[m21] = 0; matrix[m22] = zn; matrix[m23] = 0;
	matrix[m30] = -invert * (right + left) / rl;
	matrix[m31] = -invert * (top + bottom) / tb;
	matrix[m32] = -(znear + zfar) / (zfar - znear); 
	matrix[m33] = 1.f;
	identity = false;
}

void Pi3Cmatrix::SetLookAt(const vec3f &dir, const vec3f &up)
{
	vec3f xax = (up ^ dir).unit();
	vec3f yax = (dir ^ xax).unit();
	matrix[m00] = xax.x; matrix[m10] = yax.x; matrix[m20] = dir.x;
	matrix[m01] = xax.y; matrix[m11] = yax.y; matrix[m21] = dir.y;
	matrix[m02] = xax.z; matrix[m12] = yax.z; matrix[m22] = dir.z;
	identity = false;
}

void Pi3Cmatrix::Translate(const vec3f &vec)
{
	matrix[12] += vec.x;
	matrix[13] += vec.y;
	matrix[14] += vec.z;
	identity = false;
}

void Pi3Cmatrix::Translate(const float x, const float y, const float z)
{
	matrix[12] += x;
	matrix[13] += y;
	matrix[14] += z;
	identity = false;
}

void Pi3Cmatrix::move(vec3f const &offset) {
	matrix[12] = offset.x; 
	matrix[13] = offset.y; 
	matrix[14] = offset.z;
	identity = false;
}

void Pi3Cmatrix::moveXY(vec2f const &offset) {
	matrix[12] = offset.x;
	matrix[13] = offset.y;
	identity = false;
}

void Pi3Cmatrix::moveScaleXY(const float x, const float y, const float scalex, const float scaley) {
	matrix[12] = x;
	matrix[13] = y;
	matrix[m00] = scalex; 
	matrix[m11] = scaley;
	identity = false;
}

void Pi3Cmatrix::setMoveRotate(const vec3f &offset, const vec3f &rotate, const float sc)
{
	Init();
	Pi3Cmatrix rotymtx;
	rotymtx.matrix[m11] = rotymtx.matrix[m22] = cosf(rotate.x)*sc;  //x rot
	rotymtx.matrix[m21] = -(rotymtx.matrix[m12] = sinf(rotate.x)*sc);
	rotymtx.matrix[m00] = sc;

	matrix[m00] = matrix[m22] = cosf(rotate.y);
	matrix[m20] = -(matrix[m02] = sinf(rotate.y));
	
	*this = *this * rotymtx;
	Translate(offset);
	identity = false;
}

void Pi3Cmatrix::setModelMatrix(const vec3f &firstpos, const vec3f &thirdpos, const vec3f &rot)
{
	Pi3Cmatrix firstposmtx, thirdposmtx, rotmtx;
	firstposmtx.Translate(firstpos);	//model position in world
	thirdposmtx.Translate(thirdpos);	//third person position 
	rotmtx.rotateMatrixXY(rot);
	*this = firstposmtx * rotmtx * thirdposmtx;
	identity = false;
}

vec3f Pi3Cmatrix::getScale() const
{
	if (identity) return vec3f(1.f, 1.f, 1.f);
	vec3f x(matrix[m00], matrix[m01], matrix[m02]);
	vec3f y(matrix[m10], matrix[m11], matrix[m12]);
	vec3f z(matrix[m20], matrix[m21], matrix[m22]);
	return vec3f(x.length(), y.length(), z.length());
}

vec3f Pi3Cmatrix::getRotation() const
{
	//based on code from https://www.learnopencv.com/rotation-matrix-to-euler-angles/

	float sy = sqrtf(matrix[21] * matrix[21] + matrix[22] * matrix[22]);
	//float sy = sqrtf(matrix[00] * matrix[00]);

	float x, z;
	float y = atan2f(-matrix[20], sy);
	if (sy > 1e-6f) { //not singular
		x = atan2f(matrix[21], matrix[22]);
		z = atan2f(matrix[10], matrix[00]);
	}
	else {
		x = atan2f(-matrix[12], matrix[11]);
		z = 0;
	}

	if (x == PI && z == PI) {
		x = z = 0;
		y = PI - y;
	}
	else {
		if (y < 0) y = PI2 + y;
	}

	if (z < 0) z += PI2;
	if (x < 0) x += PI2;

	if (x >= PI2) x -= PI2;
	if (y >= PI2) y -= PI2;
	if (z >= PI2) z -= PI2;
	return vec3f(x, y, z);
}

vec3f Pi3Cmatrix::transformVec(const vec3f &vec) const
{
	return (identity) ? vec : vec3f(
		matrix[m00] * vec.x + matrix[m10] * vec.y + matrix[m20] * vec.z + matrix[m30],
		matrix[m01] * vec.x + matrix[m11] * vec.y + matrix[m21] * vec.z + matrix[m31],
		matrix[m02] * vec.x + matrix[m12] * vec.y + matrix[m22] * vec.z + matrix[m32]);
}

vec3f Pi3Cmatrix::transformVec(const float * verts) const
{
	return (identity) ? vec3f(verts[0], verts[1], verts[2]) : vec3f(
		matrix[m00] * verts[0] + matrix[m10] * verts[1] + matrix[m20] * verts[2] + matrix[m30],
		matrix[m01] * verts[0] + matrix[m11] * verts[1] + matrix[m21] * verts[2] + matrix[m31],
		matrix[m02] * verts[0] + matrix[m12] * verts[1] + matrix[m22] * verts[2] + matrix[m32]);
}

vec3f Pi3Cmatrix::transformVec(const float x, const float y, const float z) const
{
	return (identity) ? vec3f(x, y, z) : vec3f(
		matrix[m00] * x + matrix[m10] * y + matrix[m20] * z + matrix[m30],
		matrix[m01] * x + matrix[m11] * y + matrix[m21] * z + matrix[m31],
		matrix[m02] * x + matrix[m12] * y + matrix[m22] * z + matrix[m32]);
}

vec3f Pi3Cmatrix::transformRotateVec(const vec3f &vec) const
{
	return (identity) ? vec : vec3f(
		matrix[m00] * vec.x + matrix[m10] * vec.y + matrix[m20] * vec.z,
		matrix[m01] * vec.x + matrix[m11] * vec.y + matrix[m21] * vec.z,
		matrix[m02] * vec.x + matrix[m12] * vec.y + matrix[m22] * vec.z);
}

vec3f Pi3Cmatrix::transformRotateVec(const float * verts) const
{
	return (identity) ? vec3f(verts[0], verts[1], verts[2]) : vec3f(
		matrix[m00] * verts[0] + matrix[m10] * verts[1] + matrix[m20] * verts[2],
		matrix[m01] * verts[0] + matrix[m11] * verts[1] + matrix[m21] * verts[2],
		matrix[m02] * verts[0] + matrix[m12] * verts[1] + matrix[m22] * verts[2]);
}

vec3f Pi3Cmatrix::transformRotateVec(const float x, const float y, const float z) const
{
	return (identity) ? vec3f(x,y,z) : vec3f(
		matrix[m00] * x + matrix[m10] * y + matrix[m20] * z,
		matrix[m01] * x + matrix[m11] * y + matrix[m21] * z,
		matrix[m02] * x + matrix[m12] * y + matrix[m22] * z);
}

vec3f Pi3Cmatrix::transposeRotateVec(const vec3f &vec) const
{
	return (identity) ? vec : vec3f(
		matrix[m00] * vec.x + matrix[m01] * vec.y + matrix[m02] * vec.z,
		matrix[m10] * vec.x + matrix[m11] * vec.y + matrix[m12] * vec.z,
		matrix[m20] * vec.x + matrix[m21] * vec.y + matrix[m22] * vec.z);
}

vec3f Pi3Cmatrix::transposeRotateVec(const float * verts) const
{
	return (identity) ? vec3f(verts[0],verts[1],verts[2]) : vec3f(
		matrix[m00] * verts[0] + matrix[m01] * verts[1] + matrix[m02] * verts[2],
		matrix[m10] * verts[0] + matrix[m11] * verts[1] + matrix[m12] * verts[2],
		matrix[m20] * verts[0] + matrix[m21] * verts[1] + matrix[m22] * verts[2]);
}

vec3f Pi3Cmatrix::transposeVec(const vec3f &vec) const
{
	return transposeRotateVec(vec) - vec3f(matrix[m30], matrix[m31], matrix[m32]);
}

vec3f Pi3Cmatrix::transposeVec(const float * verts) const
{
	return transposeRotateVec(verts) - vec3f(matrix[m30], matrix[m31], matrix[m32]);
}

void Pi3Cmatrix::Init() {
	memset(&matrix,0,60);
	matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.f;
	identity = true;
}

void Pi3Cmatrix::setRotateXY(const vec3f &rot)
{
	Pi3Cmatrix rotxmtx, rotymtx;
	rotxmtx.SetRotateYbit(rot.y);
	rotymtx.SetRotateXbit(rot.x);
	*this = rotxmtx * rotymtx;
}

void Pi3Cmatrix::rotateMatrixXY(const vec3f &rot)
{
	Pi3Cmatrix rotXY;
	rotXY.setRotateXY(rot);
	*this = *this * rotXY;
	identity = false;
}

void Pi3Cmatrix::setRotate(const vec3f &rot)
{
	vec3f m = position();
	Init();
	rotate(rot);
	move(m);
}

void Pi3Cmatrix::rotate(const vec3f &rot)
{
	Pi3Cmatrix rotmtx;
	if (rot.z != 0) rotmtx.SetRotateZbit(rot.z);

	if (rot.x != 0) {
		Pi3Cmatrix rotmtxx;
		rotmtxx.SetRotateXbit(rot.x);
		rotmtx = rotmtx * rotmtxx;
	}

	if (rot.y != 0) {
		Pi3Cmatrix rotmtxy;
		rotmtxy.SetRotateYbit(rot.y);
		rotmtx = rotmtx * rotmtxy;
	}
	*this = *this * rotmtx;
	identity = false;
}

void Pi3Cmatrix::rotateAXYZ(const float angle, const float x, const float y, const float z)
{
	float cosa = cosf(angle);
	float sina = sinf(angle);
	Pi3Cmatrix rotmtx;
	rotmtx.matrix[m00] = x * x * (1 - cosa) + cosa, rotmtx.matrix[m10] = x * y * (1 - cosa) - z * sina, rotmtx.matrix[m20] = x * z * (1 - cosa) + y * sina;
	rotmtx.matrix[m01] = y * x * (1 - cosa) + z * sina, rotmtx.matrix[m11] = y * y * (1 - cosa) + cosa, rotmtx.matrix[m21] = y * z * (1 - cosa) - x * sina;
	rotmtx.matrix[m02] = z * x * (1 - cosa) - y * sina, rotmtx.matrix[m12] = z * y * (1 - cosa) + x * sina, rotmtx.matrix[m22] = z * z * (1 - cosa) + cosa;
	rotmtx.identity = false;
	//rotmtx.matrix[m11] = rotmtx.matrix[m22] = cosf(angle);
	//rotmtx.matrix[m21] = -(rotmtx.matrix[m12] = sinf(angle));

	//matrix[m00] = matrix[m22] = cosf(rotate.y);
	//matrix[m20] = -(matrix[m02] = sinf(rotate.y));

	*this = *this * rotmtx;
	identity = false;
}

void Pi3Cmatrix::scale(const vec3f& scale)
{
	Pi3Cmatrix sclmtx;
	sclmtx.SetScales(scale);
	*this = *this * sclmtx;
	identity = false;
}

//This code comes from GLU source with exception of float calcs ...

#define SWAP_ROWS_FLOAT(a, b) { float *_tmp = a; (a) = (b); (b) = _tmp; }

Pi3Cmatrix Pi3Cmatrix::inverse()
{
	Pi3Cmatrix out;
	out.identity = false;
	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;
	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];
	r0[0] = matrix[m00], r0[1] = matrix[m01], r0[2] = matrix[m02], r0[3] = matrix[m03], r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0;
	r1[0] = matrix[m10], r1[1] = matrix[m11], r1[2] = matrix[m12], r1[3] = matrix[m13], r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0;
	r2[0] = matrix[m20], r2[1] = matrix[m21], r2[2] = matrix[m22], r2[3] = matrix[m23], r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0;
	r3[0] = matrix[m30], r3[1] = matrix[m31], r3[2] = matrix[m32], r3[3] = matrix[m33], r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
	/* choose pivot - or die */
	if (fabsf(r3[0]) > fabsf(r2[0]))
		SWAP_ROWS_FLOAT(r3, r2);
	if (fabsf(r2[0]) > fabsf(r1[0]))
		SWAP_ROWS_FLOAT(r2, r1);
	if (fabsf(r1[0]) > fabsf(r0[0]))
		SWAP_ROWS_FLOAT(r1, r0);
	if (0.0 == r0[0])
		return out;
	/* eliminate first variable     */
	m1 = r1[0] / r0[0];
	m2 = r2[0] / r0[0];
	m3 = r3[0] / r0[0];
	s = r0[1];
	r1[1] -= m1 * s;
	r2[1] -= m2 * s;
	r3[1] -= m3 * s;
	s = r0[2];
	r1[2] -= m1 * s;
	r2[2] -= m2 * s;
	r3[2] -= m3 * s;
	s = r0[3];
	r1[3] -= m1 * s;
	r2[3] -= m2 * s;
	r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) {
		r1[4] -= m1 * s;
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r0[5];
	if (s != 0.0) {
		r1[5] -= m1 * s;
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r0[6];
	if (s != 0.0) {
		r1[6] -= m1 * s;
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r0[7];
	if (s != 0.0) {
		r1[7] -= m1 * s;
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}
	/* choose pivot - or die */
	if (fabsf(r3[1]) > fabsf(r2[1]))
		SWAP_ROWS_FLOAT(r3, r2);
	if (fabsf(r2[1]) > fabsf(r1[1]))
		SWAP_ROWS_FLOAT(r2, r1);
	if (0.0 == r1[1])
		return out;
	/* eliminate second variable */
	m2 = r2[1] / r1[1];
	m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2];
	r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3];
	r3[3] -= m3 * r1[3];
	s = r1[4];
	if (0.0 != s) {
		r2[4] -= m2 * s;
		r3[4] -= m3 * s;
	}
	s = r1[5];
	if (0.0 != s) {
		r2[5] -= m2 * s;
		r3[5] -= m3 * s;
	}
	s = r1[6];
	if (0.0 != s) {
		r2[6] -= m2 * s;
		r3[6] -= m3 * s;
	}
	s = r1[7];
	if (0.0 != s) {
		r2[7] -= m2 * s;
		r3[7] -= m3 * s;
	}
	/* choose pivot - or die */
	if (fabsf(r3[2]) > fabsf(r2[2]))
		SWAP_ROWS_FLOAT(r3, r2);
	if (0.0 == r2[2])
		return out;
	/* eliminate third variable */
	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
		r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];
	/* last check */
	if (0.0f == r3[3])
		return out;
	s = 1.0f / r3[3];		/* now back substitute row 3 */
	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;
	m2 = r2[3];			/* now back substitute row 2 */
	s = 1.0f / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
		r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
		r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
		r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;
	m1 = r1[2];			/* now back substitute row 1 */
	s = 1.0f / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
		r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
		r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;
	m0 = r0[1];			/* now back substitute row 0 */
	s = 1.0f / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
		r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	out.matrix[m00] = r0[4], out.matrix[m01] = r0[5], out.matrix[m02] = r0[6], out.matrix[m03] = r0[7];
	out.matrix[m10] = r1[4], out.matrix[m11] = r1[5], out.matrix[m12] = r1[6], out.matrix[m13] = r1[7];
	out.matrix[m20] = r2[4], out.matrix[m21] = r2[5], out.matrix[m22] = r2[6], out.matrix[m23] = r2[7];
	out.matrix[m30] = r3[4], out.matrix[m31] = r3[5], out.matrix[m32] = r3[6], out.matrix[m33] = r3[7];
	return out;
}
