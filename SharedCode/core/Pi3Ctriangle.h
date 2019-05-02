#pragma once

#include"Pi3Cvector.h"

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

class Pi3Ctriangle {

public:
	Pi3Ctriangle();
	Pi3Ctriangle(const vec3f v1, const vec3f v2, const vec3f v3) { this->v1 = v1; this->v2 = v2; this->v3 = v3; }
	Pi3Ctriangle(const float v1x, const float v1y, const float v1z, const float v2x, const float v2y, const float v2z, const float v3x, const float v3y, const float v3z)
		{ v1=vec3f(v1x,v1y,v1z); v2=vec3f(v2x,v2y,v2z); v3=vec3f(v3x,v3y,v3z); }
		
	vec3f v1,v2,v3;

	bool pointInBox(const vec3f p);
	bool pointInBoxXZ(const vec3f p);
	float rayIntersect(vec3f pos, vec3f dir);
	vec3f faceNormal();
	vec3f reflection(vec3f dir);
};

