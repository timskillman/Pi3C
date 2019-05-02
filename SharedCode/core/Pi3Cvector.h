#pragma once

#include <stdint.h>

#ifdef __WINDOWS__
#include <cmath>
#else
#include <math.h>
#endif

// =======================================================================
// Pi3C Graphics Library
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

/*  Vectorf contains the following floating point functions:-

	vec2f		x,y vector
	vec3f		x,y,z vector
	vec4f		x,y,z,w vector

*/

///// vec2f class ///////////////////////////////////////////////////////////////////////////////////////

#define PI 3.14159268f
#define PI2 6.28318530718f
#define DEG2RAD 0.0174533f
#define RAD2DEG 57.2958f

class vec2f {
public:
	vec2f() : x(0), y(0) {}
	vec2f(float x, float y) : x(x), y(y) {}

	void Normalise()
	{
		float v = length();
		x = x / v;
		y = y / v;
	}

	vec2f operator - () const
	{
		return vec2f(-x, -y);
	}

	vec2f operator - (vec2f rhs)
	{
		return vec2f(x - rhs.x, y - rhs.y);
	}

	vec2f operator + (const vec2f &rhs) const
	{
		return vec2f(x + rhs.x, y + rhs.y);
	}

	bool operator == (vec2f rhs)
	{
		return (x == rhs.x && y == rhs.y);
	}

	bool operator != (vec2f rhs)
	{
		return (x != rhs.x || y != rhs.y);
	}

	vec2f operator*(float scale)
	{
		return vec2f(x * scale, y * scale);
	}

	vec2f operator/(float div)
	{
		return vec2f(x / div, y / div);
	}

	vec2f & operator += (vec2f const & v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	vec2f & operator -= (vec2f const & v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2f & operator *= (float v)
	{
		x *= v;
		y *= v;
		return *this;
	}

	float length() const
	{
		return sqrtf(x*x + y*y);
	}


	static vec2f g_Zero;

	float x;
	float y;
};

///// vec3f class ///////////////////////////////////////////////////////////////////////////////////////

class vec3f {
public:
	vec3f() : x(0.f), y(0.f), z(0.f) {}

	vec3f(float x, float y, float z) : x(x), y(y), z(z) {}

	inline vec3f operator - () {
		return vec3f(-x, -y, -z);
	}

	inline vec3f operator - () const {
		return vec3f(-x, -y, -z);
	}

	inline vec3f operator - (vec3f rhs)
	{
		return vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	inline vec3f operator - (vec3f rhs) const
	{
		return vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	inline vec3f operator + (vec3f rhs)
	{
		return vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline const vec3f operator + (const vec3f rhs) const
	{
		return vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline vec3f & operator += (const vec3f &v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	inline vec3f & operator -= (const vec3f &v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	inline bool operator==(vec3f rhs) {
		return (x == rhs.x && y == rhs.y && z == rhs.z);
	}

	inline vec3f operator * (float scale) const {
		return vec3f(x * scale, y * scale, z * scale);
	}

	inline vec3f operator * (vec3f rhs) const {
		return vec3f(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	inline vec3f operator *= (vec3f rhs) {
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
		return *this;
	}

	inline vec3f operator /= (vec3f rhs) {
		x /= rhs.x; y /= rhs.y; z /= rhs.z;
		return *this;
	}

	//inline float operator * (vec3f t) // scalar product
	//{
	//	return x*t.x + y*t.y + z*t.z;
	//}
	
	inline vec3f operator / (float div) const {
		return vec3f(x / div, y / div, z / div);
	}

	inline vec3f operator ^ (const vec3f &t) const {
		return vec3f(y*t.z - z * t.y, t.x*z - x * t.z, x*t.y - y * t.x);  // cross product
	}

	inline float scalar() const {
		return x * x + y * y + z * z;
	}

	inline float length() const {
		return sqrtf(x*x + y*y + z*z);
	}

	inline vec3f unit() const // normalised to a length of 1
	{
		float l = length();
		if (l == 0.0) return vec3f(0.0, 0.0, 0.0); //avoid divide by zero error
		return vec3f(x / l, y / l, z / l);
	}
	
	void normalise() // normalised to a length of 1
	{
		float l = length();
		if (l != 0.0) {
			x = x / l; y = y / l; z = z / l;
		}
	}

	inline vec3f trinormal(vec3f v2, vec3f v3) const //calculate triangle normal (normalised)
	{
		return ((v2 - *this) ^ (v3 - *this)).unit();
	}

	inline bool isZero() const {
		return ((x == 0.f) && (y == 0.f) && (z == 0.f));
	}

	//static vec3f g_Zero;

	float x;
	float y;
	float z;
};

///// vec3f class ///////////////////////////////////////////////////////////////////////////////////////

class vec4f {
public:
	vec4f() : x(0.f), y(0.f), z(0.f), w(0.f) {
	}

	vec4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	inline vec4f operator - ()
	{
		return vec4f(-x, -y, -z, -w);
	}

	inline vec4f operator - (vec4f rhs)
	{
		return vec4f(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}

	inline vec4f operator + (vec4f rhs)
	{
		return vec4f(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}

	inline vec4f & operator += (vec4f const & v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	inline bool operator==(vec4f rhs)
	{
		return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
	}

	inline vec4f operator*(float scale)
	{
		return vec4f(x * scale, y * scale, z * scale, w * scale);
	}

	inline float operator * (vec4f t) // scalar product
	{
		return x*t.x + y*t.y + z*t.z + w*t.w;
	}

	inline vec4f operator/(float div)
	{
		return vec4f(x / div, y / div, z / div, w / div);
	}

	inline float length()
	{
		return sqrtf(x*x + y*y + z*z + w*w);
	}

	inline vec4f unit() // normalized to a length of 1
	{
		float l = length();
		if (l == 0.0f) return vec4f(0.0f, 0.0f, 0.0f, 0.0f);
		return vec4f(x / l, y / l, z / l, w / l);
	}

	inline bool IsZero() const
	{
		return ((x == 0.f) && (y == 0.f) && (z == 0.f) && (w == 0.f));
	}

	//static vec4f g_Zero;

	float x;
	float y;
	float z;
	float w;
};



///// vec3 (double precision) class ///////////////////////////////////////////////////////////////////////////////////////

class vec3 {
public:
	vec3() : x(0), y(0), z(0) {}

	vec3(double x, double y, double z) : x(x), y(y), z(z) {}

	inline vec3 operator - () {
		return vec3(-x, -y, -z);
	}

	inline vec3 operator - (vec3 rhs)
	{
		return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	inline vec3 operator + (vec3 rhs)
	{
		return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	inline vec3 & operator += (vec3 const & v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	inline vec3 & operator -= (vec3 const & v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	inline bool operator==(vec3 rhs) {
		return (x == rhs.x && y == rhs.y && z == rhs.z);
	}

	inline vec3 operator * (double scale) {
		return vec3(x * scale, y * scale, z * scale);
	}

	inline vec3 operator * (vec3 rhs) {
		return vec3(x * rhs.x, y * rhs.y, z * rhs.z);
	}

	inline vec3 operator *= (vec3 rhs) {
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
		return *this;
	}

	inline vec3 operator /= (vec3 rhs) {
		x /= rhs.x; y /= rhs.y; z /= rhs.z;
		return *this;
	}

	inline vec3 operator / (double div) {
		return vec3(x / div, y / div, z / div);
	}

	inline vec3 operator ^ (const vec3 &t) {
		return vec3(y*t.z - z * t.y, t.x*z - x * t.z, x*t.y - y * t.x);  // cross product
	}

	inline double length() {
		return sqrt(x*x + y * y + z * z);
	}

	inline vec3 unit() // normalized to a length of 1
	{
		double l = length();
		if (l == 0.0) return vec3(0.0, 0.0, 0.0);
		return vec3(x / l, y / l, z / l);
	}

	void normalise() // normalized to a length of 1
	{
		double l = length();
		if (l != 0.0) {
			x = x / l; y = y / l; z = z / l;
		}
	}

	inline bool isZero() const {
		return ((x == 0) && (y == 0) && (z == 0));
	}

	//static vec3 g_Zero;

	double x;
	double y;
	double z;
};
