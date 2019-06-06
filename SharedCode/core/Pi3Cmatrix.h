#pragma once

#include "Pi3CGL.h"
#include "Pi3Cvector.h"
#include <stdint.h>
#include <vector>

// =======================================================================
// Pi3C Raspberry Pi Graphics Library
// =======================================================================
//
// The MIT License
//
// Copyright (c) 2018 Tim Skillman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================

class Pi3Cmatrix {
public:
	Pi3Cmatrix() { Init(); }
	
	enum { 	m00 =0, m01 = 1, m02 = 2, m03 = 3,
			m10 =4, m11 = 5, m12 = 6, m13 = 7,
			m20 =8 ,m21 = 9, m22 = 10,m23 = 11,
			m30 =12,m31 = 13,m32 = 14,m33 = 15 };
			
	Pi3Cmatrix operator * (Pi3Cmatrix const &m2) const;
	
	void Init();
	void SetScale(const float &scale);
	void SetScales(const vec3f &scales);
	void SetRotateX(const float &angle);
	void SetRotateY(const float &angle);
	void SetRotateZ(const float &angle);
	void SetRotateXbit(const float &angle);
	void SetRotateYbit(const float &angle);
	void SetRotateZbit(const float &angle);
	void SetPerspective(const int width, const int height, const float pspmul, const float znear, const float zfar);
	void SetOrtho(const float left, const float right, const float top, const float bottom, const float znear, const float zfar, const float invert = 1.0f);
	void Translate(const vec3f &offset);
	void Translate(const float x, const float y, const float z);
	void move(const vec3f &offset);
	void moveXY(const vec2f &offset);
	void moveScaleXY(const float x, const float y, const float scalex, const float scaley);
	void setMoveRotate(const vec3f &offset, const vec3f &rotate, const float sc = 1.f);
	void setModelMatrix(const vec3f &firstpos, const vec3f &thirdpos, const vec3f &rot);
	void SetLookAt(vec3f const &dir, vec3f const &up);
	void rotateMatrixXY(const vec3f &rot);
	void rotate(const vec3f &rot);
	void setRotate(const vec3f &rot);
	void setRotateXY(const vec3f &rot);

	vec3f getScale() const;
	vec3f getRotation() const;

	inline float x() const { return matrix[m30]; }
	inline float y() const { return matrix[m31]; }
	inline float z() const { return matrix[m32]; }

	inline void setx(const float x) { matrix[m30] = x; identity = false; }
	inline void sety(const float y) { matrix[m31] = y; identity = false; }
	inline void setz(const float z) { matrix[m32] = z; identity = false; }

	vec3f transformVec(vec3f const &vec) const;						//transform vertex
	vec3f transformVec(const float x, const float y, const float z) const;
	vec3f transformVec(const float * verts) const;
	vec3f transformRotateVec(vec3f const &vec) const;				//transform vertex
	vec3f transformRotateVec(const float x, const float y, const float z) const;
	vec3f transformRotateVec(const float * verts) const;

	inline float transformX(vec3f const &v) const { return matrix[m00] * v.x + matrix[m10] * v.y + matrix[m20] * v.z + matrix[m30]; }
	inline float transformY(vec3f const &v) const { return matrix[m01] * v.x + matrix[m11] * v.y + matrix[m21] * v.z + matrix[m31]; }
	inline float transformZ(vec3f const &v) const { return matrix[m02] * v.x + matrix[m12] * v.y + matrix[m22] * v.z + matrix[m32]; }

	inline float transformX(float const *v) const { return matrix[m00] * v[0] + matrix[m10] * v[1] + matrix[m20] * v[2] + matrix[m30]; }
	inline float transformY(float const *v) const { return matrix[m01] * v[0] + matrix[m11] * v[1] + matrix[m21] * v[2] + matrix[m31]; }
	inline float transformZ(float const *v) const { return matrix[m02] * v[0] + matrix[m12] * v[1] + matrix[m22] * v[2] + matrix[m32]; }

	vec3f transposeVec(vec3f const &vec) const;
	vec3f transposeVec(const float * verts) const;
	vec3f transposeRotateVec(const vec3f &vec) const;
	vec3f transposeRotateVec(const float * verts) const;

	Pi3Cmatrix inverse();

	const vec3f position() const { return vec3f(matrix[m30], matrix[m31], matrix[m32]); }

	const float * get() const { return &matrix[0]; }

private:
	bool identity = true; //used to avoid time wasted multiplying identity matrices!
	float matrix[16] = {};
};