#pragma once

#include "Pi3Cmesh.h"
#include "Pi3Ctexture.h"
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
	Pi3Cmesh rectLine(const vec2f &pos, const vec2f &size, const float thickness = 0, const vec2f &uvpos = vec2f(0.f, 0.f), const vec2f &uvsize = vec2f(1.f, 1.f));
	Pi3Cmesh quad(const vec3f &p1, const vec3f &p2, const vec3f &p3, const vec3f &p4, const vec2f &uvpos = vec2f(0.f, 0.f), const vec2f &uvsize = vec2f(1.f, 1.f));
	Pi3Cmesh plane(const vec3f &pos, const vec2f &size, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f &uvsize = vec2f(1.f, 1.f));
	Pi3Cmesh grid(const vec2f &size = vec2f(100.f, 100.f), const float majorStep = 10.f, const float minorStep = 1.f, const uint32_t mincol = 0x606060, const uint32_t majcol = 0x404040);
	Pi3Cmesh cuboid(const vec3f &pos, const vec3f &size, const int divx=1, const int divy=1, const int divz=1);
	Pi3Cmesh lathe(const std::string &name, const vec3f &pos, std::vector<vec2f> &path, const int edges= Pi3C_SIDES,
		const float rise=0.0f, const float startAngle = 0.f, const float endAngle = 2.f*PI, const bool invert = false, const float creaseAngle = 60.0f, const vec2f &prevPoint = vec2f(0,0),
		const vec2f &lastPoint = vec2f(0, 0), const uint32_t uvtype = UVMAP_CYLINDER);
	Pi3Cmesh sphere(const vec3f &pos, const float radius, const float hemi=0.0f, const int slices= Pi3C_SIDES/2, const int sides = Pi3C_SIDES, const bool invert = false);
	Pi3Cmesh torus(const vec3f &pos, const float radius, const float ringradius, const int ringrots= Pi3C_SIDES, const int sides = Pi3C_SIDES);
	Pi3Cmesh spring(const vec3f &pos, const float radius, const float height, const int coils, const float coilradius, const int ringrots=12, int sides = Pi3C_SIDES);
	Pi3Cmesh cone(const vec3f &pos, const float radius, const float height, const int sides = Pi3C_SIDES);
	Pi3Cmesh tcone(const vec3f &pos, const float botradius, const float topradius, const float height, const int sides = Pi3C_SIDES);
	Pi3Cmesh cylinder(const vec3f &pos, const float radius, const float height, const int sides = Pi3C_SIDES);
	Pi3Cmesh tube(const vec3f &pos, const float innerradius, const float outerradius, const float height, int sides = Pi3C_SIDES);
	Pi3Cmesh disk(const vec3f &pos, const float innerRad, const float outerRad, const int sides = Pi3C_SIDES);

	Pi3Cmesh extrude(const std::string &name, const vec3f &pos, std::vector<std::vector<float>> &contours, const float depth, const uint16_t divs = 1);
	Pi3Cmesh elevationMap(Pi3Ctexture &tex, const vec3f &pos, const vec3f &size, const uint32_t xdivs, const uint32_t ydivs, const int direction, const vec2f &uvsize = vec2f(1.f, 1.f));

	//Pi3Cmesh capsule(vec3f pos, float radius, float midlength = 2.0f, int slices = Pi3C_SIDES, int sides = Pi3C_SIDES);

	void texMap(std::vector<float> &vertBuffer, uint32_t vc, uint32_t stride, uint32_t uvoffset, uint8_t x, uint8_t y);

}

