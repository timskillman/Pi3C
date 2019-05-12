#pragma once

#include "Pi3CGL.h"
#include "Pi3Cvector.h"
#include "Pi3Cmatrix.h"
#include "Pi3Cbbox3d.h"
#include "Pi3Ctouch.h"
#include <string>
#include <stdint.h>
#include <vector>

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

#define defaultStride 8

class Pi3Cmesh {
public:

	Pi3Cmesh(const std::string &name = "", const uint32_t stride = defaultStride);
	~Pi3Cmesh() {}

	enum vertRefs { v_x=0 ,v_y=1 ,v_z=2 ,n_x=3, n_y=4, n_z=5, v_u=6, v_v=7 };
//  STRUCTS

	struct vertVNU {
		float vx;
		float vy;
		float vz;
		float nx;
		float ny;
		float nz;
		float ux;
		float uy;
	};

	//struct meshHeader {
		std::string name;
		Pi3Cbbox3d bbox;
		uint16_t mode;
		std::vector<float> verts;
		GLint vertOffset;
		uint32_t vertSize;
		uint32_t stride;
		int32_t materialRef;
		size_t vc;
		GLint bufRef;
		float animval;											//Each tween must have the same number of vertices as the verts buffer since the verts buffer will hold the results
		bool collisionMesh;		//if set, then this mesh is collision only
		bool dynamic;			//if set, mesh can be modified in realtime and verts are not thrown away
	//};

/// FUNCTIONS ...

	void addPackedVert(const vec3f &position, const vec3f &normal, const vec2f &uv, const GLfloat *cols);
	void addRectangle(std::vector<float> &verts, const vec3f &pos, const vec3f &size, const vec2f &uv, const vec2f &us);
	void updateRectCoords(std::vector<float> &verts, const vec3f &pos, const vec3f &size);
	void updateRectTransformCoords(std::vector<float> &verts, const vec3f &pos, const vec3f &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent); //const vec2f &uv, const vec2f &us,

	void offset(const vec3f pos, std::vector<float> &verts, const uint32_t start, const uint32_t size, const uint32_t stride = defaultStride);
	void resize(const vec3f &pos, const vec3f &size, const std::vector<float> &mverts);
	void reset();

	void render(const GLenum mode = GL_TRIANGLES);

	int32_t touchPoint(Pi3Ctouch &touchObj, const Pi3Cmatrix &mtx, const std::vector<float> &mverts) const;		//returns the index of triangle touched and the intersection point

	Pi3Cmesh clone(const uint32_t from, const uint32_t size) const;

	//Create index reference list for shared triangle points ...
	struct VertsIndsUVs {
		std::vector<float> &verts;
		std::vector<uint32_t> &indexes;
		std::vector<float> &uvs;
		std::vector<uint32_t> &uvindexes;
	};

	void createSharedTriangleList(const std::vector<float> &verts, std::vector<uint32_t> &vertindexes, std::vector<float> &newverts, std::vector<uint32_t> &uvindexes, std::vector<float> &newuvs, const float tolerance = 0.0005f);
	void createSharedTriangleList(VertsIndsUVs *in, VertsIndsUVs *out, const float tolerance = 0.0005f);

	void updateNormals(const uint32_t min, const uint32_t max);

	//bool collideVector(bool bounce, vec3f &pos, vec3f &dir);	//returns hit and modified pos, dir vectors
	//float collideFloor(vec3f pos, float &prevHeight); 			//returns height above the floor

/// VARIABLES ...

	//meshHeader header;

	std::vector< std::vector<uint32_t>> sharedIndexes;
	std::vector<std::vector<float>> tweenverts;				//Holds tweening vertices for animating the mesh
	std::vector<float> tweenlengths;

private:
	void init(std::string _name, uint32_t stride) {
		reset();
		name = _name;
		this->stride = stride;
	}

	float triArea(const vec3f &v1, const vec3f &v2, const vec3f &v3, float &maxLength) const;

};


