#pragma once

#include "Pi3Cmesh.h"
#include <vector>
#include <string>

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

#define Pi3C_SIDES 24

namespace Pi3Cshapes {

	/* Class that creates all 3D objects
	*/

	enum { UVMAP_CYLINDER, UVMAP_SPHERE };
	enum { FACE_FRONT, FACE_BACK, FACE_RIGHT, FACE_LEFT, FACE_TOP, FACE_BOTTOM };

	Pi3Cmesh rect(const vec2f &pos, const vec2f &size, const vec2f &uvpos = vec2f(0.f, 0.f), const vec2f &uvsize = vec2f(1.f,1.f));
	Pi3Cmesh plane(const vec3f &pos, const vec2f &size, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f &uvsize = vec2f(1.f, 1.f));
	Pi3Cmesh cuboid(const vec3f &pos, const vec3f &size, const int divx=1, const int divy=1, const int divz=1);
	Pi3Cmesh lathe(std::string name, vec3f pos, std::vector<vec2f> &path, int sides= Pi3C_SIDES,
		float rise=0.0f, float startAngle = 0.f, float endAngle = 2.f*PI, bool invert = false, float creaseAngle = 60.0f, vec2f prevPoint = vec2f(0,0),
		vec2f lastPoint = vec2f(0, 0), uint32_t uvtype = UVMAP_CYLINDER);
	Pi3Cmesh sphere(vec3f pos, float radius, float hemi=0.0f, int slices= Pi3C_SIDES, int sides = Pi3C_SIDES, bool invert = false);
	Pi3Cmesh torus(vec3f pos, float radius, float ringradius, int ringrots=12, int sides = Pi3C_SIDES);
	Pi3Cmesh spring(vec3f pos, float radius, float height, int coils, float coilradius, int ringrots=12, int sides = Pi3C_SIDES);
	Pi3Cmesh cone(vec3f pos, float radius, float height, int sides = Pi3C_SIDES);
	Pi3Cmesh tcone( vec3f pos, float botradius, float topradius, float height, int sides = Pi3C_SIDES);
	Pi3Cmesh cylinder( vec3f pos, float radius, float height, int sides = Pi3C_SIDES);
	Pi3Cmesh tube(vec3f pos, float innerradius, float outerradius, float height, int sides = Pi3C_SIDES);
	Pi3Cmesh disc(vec3f pos, float innerRad, float outerRad, int sides = Pi3C_SIDES);

	Pi3Cmesh extrude(std::string name, vec3f pos, std::vector<std::vector<vec2f>> &contoursxy, float depth = 1.0f, uint16_t divs = 1);
	Pi3Cmesh elevationMap(uint8_t * map, uint32_t w, uint32_t h, uint32_t bpp, const vec3f pos, vec2f size, float mapHeight, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f uvsize = vec2f(1.f, 1.f));
	
	//Pi3Cmesh capsule(vec3f pos, float radius, float midlength = 2.0f, int slices = Pi3C_SIDES, int sides = Pi3C_SIDES);

	void texMap(std::vector<float> &vertBuffer, uint32_t vc, uint32_t stride, uint32_t uvoffset, uint8_t x, uint8_t y);

}

