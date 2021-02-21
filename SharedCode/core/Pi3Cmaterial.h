#pragma once

#include "Pi3CGL.h"
#include "Pi3Cvector.h"
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

class Pi3Cmaterial {
public:
	Pi3Cmaterial();

	void init(const std::string &name = "");
	void SetColAmbient(const uint32_t colour);
	void SetColAmbient(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
	void SetColEmissive(const uint32_t colour);
	void SetColEmissive(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
	void SetColDiffuse(const uint32_t colour);
	void SetColDiffuse(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
	void SetColDiffuse(const vec4f &col);
	void SetColSpecular(const uint32_t colour);
	void SetColSpecular(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
	void SetSpecularPower(const uint32_t s) { shininess = (float)s / 255.0f; changed = true; }
	void SetAlphaMode(const GLenum mode) { }
	void SetCullMode(const int32_t cm) { cullface = (cm == 1); changed = true; }
	void SetAnimationTexture(const float stepx, const float stepy);
	void SetAnimationSheet(const int divx, const int divy, const int frames = 0);
	void AnimateTexture(const float ticks = 1.f);

	uint32_t GetColAmbient();
	uint32_t GetColEmissive();
	uint32_t GetColDiffuse();
	uint32_t GetColSpecular();

	enum {
		ALPHA_NONE = 0,
		ALPHA_HALF = 1,
		ALPHA_ADD = 2,
		ALPHA_SUB = 3,
		ALPHA_BLEND = 4,
		ALPHA_DEFAULT = 5
	};

	enum {
		CULL_FRONT,
		CULL_BACK,
		CULL_NONE
	};

	std::string name;
	int illum;
	
	//texture settings ...
	std::string texName;
	int32_t texID;	//GL texture ID (used for textures outside Resources textures pool, e.g. fonts)
	int32_t texRef; //ref to stored texture in meshpool
	//std::string texBumpName;
	int32_t texBumpRef;	//GL texture ID
	uint32_t texWidth;
	uint32_t texHeight;
	
	vec2f animvec; 			//cheap texture animation incrementals
	vec2f animoffset;  		//cheap texture animation offset
	vec2f animsize;
	int animFrames;			//If set to 0, then animate whole sprite sheet
	int animFrameCount;
	float frameticks;
	float framespeed;

//private:
	vec4f colAmbient;
	vec4f colDiffuse;
	vec4f colEmissive;
	vec4f colSpecular;
	float shininess;
	float alpha;
	float alphaTestValue;
	int reflective;		//0.0 = no reflection, 1.0 = full reflection (currently only 0 or 1)

	bool cullface = true;
	GLenum rendermode;
	bool changed = true;
	int32_t groupID = 0;	//used for separating groups of materials - e.g. system, models, skyboxes etc..

};

