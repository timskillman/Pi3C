#pragma once

#include "Pi3CGL.h"
#include "Pi3Cvector.h"
#include "Pi3Cmatrix.h"
#include "Pi3Cbbox3d.h"
#include "Pi3Ctouch.h"
#include "Pi3CvertsPtr.h"
#include <string>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <cmath>

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

#define defaultStride 9


class Pi3Cmesh {
public:

	Pi3Cmesh(const std::string &name = "", const uint32_t stride = defaultStride);
	~Pi3Cmesh() {}

	//enum vertRefs { v_x = 0, v_y = 1, v_z = 2, n_x = 3, n_y = 4, n_z = 5, v_u = 6, v_v = 7 };
	//  STRUCTS

	//struct vertVNU {
	//	float vx;
	//	float vy;
	//	float vz;
	//	float nx;
	//	float ny;
	//	float nz;
	//	float ux;
	//	float uy;
	//};

	//struct meshHeader {
	std::string name;
	Pi3Cbbox3d bbox;
	uint16_t mode;
	std::vector<float> verts;
	//std::vector<float> overts;	//outline vertices
	std::vector<uint32_t> lineIndexes;
	GLint vertOffset;
	uint32_t vertSize;
	uint32_t stride;
	uint32_t vc;
	GLint bufRef;
	int32_t materialRef;
	float animval;			//Each tween must have the same number of vertices as the verts buffer since the verts buffer will hold the results
//};

	struct VertsIndsUVs {
		std::vector<float>& verts;
		std::vector<uint32_t>& indexes;
		std::vector<float>& uvs;
		std::vector<uint32_t>& uvindexes;
	};


/// FUNCTIONS ...

	void addPackedVert(const vec3f &position, const vec3f &normal, const vec2f &uv, const uint32_t col);

	void offset(const vec3f pos, std::vector<float> &verts, uint32_t start, const uint32_t size, const uint32_t stride = defaultStride);
	void resize(const vec3f &pos, const vec3f &size, const std::vector<float> &mverts);
	void transform(std::vector<float>& verts, const uint32_t vo, const Pi3Cmatrix& mtx);
	void reset();

	void render(const GLenum rendermode = GL_TRIANGLES);
	void renderIndexed(const GLenum rendermode, uint32_t indexCo, uint32_t * indexes);

	int32_t touchPoint(Pi3Ctouch &touchObj, const Pi3Cmatrix &mtx, const std::vector<float> &mverts) const;		//returns the index of triangle touched and the intersection point

	Pi3Cmesh clone(const uint32_t from, const uint32_t size) const;


	void addRect(const vec3f &pos, const vec2f &size, const vec2f &uv = vec2f(0, 0), const vec2f &us = vec2f(1.f, 1.f));
	void addXshape(const vec3f& pos, const vec2f& size, const vec2f& uv = vec2f(0, 0), const vec2f& us = vec2f(1.f, 1.f));
	void updateRectCoords2D(std::vector<float> &verts, uint32_t &p, const float x, const float y, const float w, const float h);
	void updateRectCoordsBB(std::vector<float> &verts, uint32_t &p, const vec3f &pos, const float ax, const float az, const float sy);
	void updateRectCoordsPSP(std::vector<float> &verts, uint32_t &p, const float x, const float y, const float z, const float cx, const float cy, const float ax, const float sy, const float w);
	void updateRectCoordsROT(std::vector<float> &verts, uint32_t &p, const float x, const float y, const float ax, const float ay);

	uint32_t calcRectPtr(uint32_t ref) { return (vertOffset + ref * 6) * stride; }

	void updateNormals(const uint32_t min, const uint32_t max);
	void updateBounds(const vertsPtr* vp = nullptr);

	bool hasColliderGrid = false;
	float checkColliderGrid(const vec3f &p, const Pi3Cmatrix &mtx, float prevHeight);
	bool createColliderGrid();

	//void createOutlines();

	void createSharedTriangleList(const std::vector<float>& verts, std::vector<uint32_t>& vertindexes, std::vector<float>& newverts, std::vector<uint32_t> &normindexes, std::vector<float> &newnorms, std::vector<uint32_t>& uvindexes, std::vector<float>& newuvs, const float tolerance = 0.0005f);
	void createSharedTriangleList(VertsIndsUVs* in, VertsIndsUVs* out, const float tolerance = 0.0005f);
	void createTriangleEdges(std::vector<float> &verts);

	//bool collideVector(bool bounce, vec3f &pos, vec3f &dir);	//returns hit and modified pos, dir vectors
	//float collideFloor(vec3f pos, float &prevHeight); 			//returns height above the floor

/// VARIABLES ...

	//meshHeader header;
	//std::vector<std::vector<uint32_t>> sharedIndexes;
	//std::vector<std::vector<float>> tweenverts;				//Holds tweening vertices for animating the mesh
	//std::vector<float> tweenlengths;

private:
	void init(std::string _name, uint32_t stride) {
		reset();
		name = _name;
		this->stride = stride;
		materialRef = 0; //default material
	}

	float triArea(const vec3f &v1, const vec3f &v2, const vec3f &v3, float &maxLength) const;

	std::vector<float> xgrid[10][10]; //collider grid ... stores triangles grouped in a 10x10 area - used for fast ground collision testing

};
