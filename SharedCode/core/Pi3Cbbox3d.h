#pragma once

#include <stdint.h>
#include "Pi3Cvector.h"
#include "Pi3Cmatrix.h"
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

class Pi3Cbbox3d {
public:
	Pi3Cbbox3d() {
		init();
	}
	Pi3Cbbox3d(const vec3f origin, const vec3f size);
	~Pi3Cbbox3d() {}

	void init();
	void update(const vec3f &point);
	void update(const Pi3Cbbox3d &box, const Pi3Cmatrix* mtx);
	void update(const Pi3Cbbox3d &box);
	void set(const Pi3Cbbox3d &box, const Pi3Cmatrix* mtx);
	void bboxFromVerts(const std::vector<float> &verts, const uint32_t start, const uint32_t size, const uint32_t stride);
	void bboxFromTVerts(const std::vector<float> &verts, const Pi3Cmatrix* mtx, const uint32_t start, const uint32_t size, const uint32_t stride);
	void radiusFromVerts(const std::vector<float> &verts, const vec3f centre, const uint32_t start, const uint32_t size, const uint32_t stride);
	float radiusFromTVerts(const std::vector<float> &verts, const vec3f centre, const Pi3Cmatrix* mtx, const uint32_t start, const uint32_t size, const uint32_t stride) const;
	Pi3Cbbox3d bboxFromTVerts(const Pi3Cmatrix* mtx) const;
	float width() const { return max.x - min.x; }
	float depth() const { return max.z - min.z; }
	float height() const { return max.y - min.y; }
	vec3f center() const { return (min+max) * 0.5f; }
	float calcradius() const {
		float w = width() / 2.f, h = height() / 2.f, d = depth() / 2.f;
		return sqrtf(w*w + h*h + d*d);
	}
	vec3f size() { return vec3f(max.x - min.x, max.y - min.y, max.z - min.z); }
	void translate(const vec3f t);
	void moveto(const vec3f p);
	bool valid() const { return (!(min.x > max.x)); }
	
	bool pointInside(vec3f p) const {
		return ((p.x > min.x && p.x < max.x) && (p.y > min.y && p.y < max.y) && (p.z > min.z && p.z < max.z)); }
	bool pointInsideXZ(vec3f p) const {
		return ((p.x > min.x && p.x < max.x) && (p.z > min.z && p.z < max.z)); }
	bool pointInsideXZtx(const vec3f &p, const Pi3Cmatrix &mtx) const {
		Pi3Cbbox3d box;
		box.update(*this, &mtx);
		return ((p.x > box.min.x && p.x < box.max.x) && (p.z > box.min.z && p.z < box.max.z));
	}

	vec3f min;
	vec3f max;
	float radius;
};
