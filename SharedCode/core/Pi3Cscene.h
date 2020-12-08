#pragma once

#include "Pi3Cresource.h"
#include "Pi3Cmodel.h"
#include "Pi3Cshader.h"
#include "Pi3Cvector.h"
#include "Pi3Ctouch.h"
#include "Pi3Crect.h"
#include "Pi3Cmaterial.h"
#include "Pi3CviewInfo.h"
#include <vector>
#include <functional>

constexpr float PSPVALUE = 800.f;

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

/* 
 * Class containing a scene of models, avatars, cameras, lights etc..
 */

class Pi3Cscene {
public:
	Pi3Cscene() { }
	Pi3Cscene(Pi3Cresource *resource) { 
		init(resource);
	}

	~Pi3Cscene() {}

	Pi3Cresource *resource = nullptr; //Ptr to resources (set upon initialisation)
	

	std::vector<Pi3Cmodel> models;
	std::vector<Pi3Cmodel> models2D;
	std::vector<Pi3Cmodel> skyboxes;
	//std::vector<Pi3Ccamera> cameras;
	//std::vector<Pi3Clight> lights;
	
	//std::vector<Pi3Cavatar> avatar;	//several avatars

	void init(Pi3Cresource *resource) { 
		this->resource = resource; 
		selectMaterial = *resource->defaultMaterial();
		selectMaterial.SetColDiffuse(0xffffff);
		selectMaterial.SetColEmissive(0xffffff);
		selectMaterial.SetColAmbient(0xffffff);
		selectMaterial.rendermode = GL_LINE_STRIP;
		selectMaterial.illum = 0;
	}
	void setMatrix(vec3f const &firstpos, vec3f const &thirdpos, vec3f const &rot);
	void render3D(const float ticks = 1.f, Pi3Cmaterial *materialOverride = nullptr);
	void render2D(const float ticks = 1.f);
	void renderSkybox(const vec3f &pos, const int32_t skyboxref = 0, float ticks = 1.f);
	void renderOffscreen(const viewInfo& viewinfo, Pi3Cmaterial* outlines, int width = 0, int height = 0); //, const Pi3Cshader &shader
	void renderView(const viewInfo& view, Pi3Cmaterial* outlines = nullptr);

	void checkFBerrors();
	void flipImage(std::vector<uint8_t> &src, std::vector<uint8_t> &dest, uint32_t w, uint32_t h);
	void saveBufferToPNG(const char * filename, std::vector<uint8_t> &snapShot, const int width, const int height);

	bool has2Dmodels() { return models2D.size() > 0; }
	Pi3Cmodel* lastModel() { return &models[models.size() - 1]; };

	Pi3Cmodel * getSubModel(const int32_t * groupRefs, const int32_t maxlevel);
	Pi3Cmatrix * getModelMatrix() { return &modelMatrix3D; }
	Pi3Cbbox3d getSelectedBounds();

	Pi3Ctouch touch(const vec3f &mousexyz, const bool calcPerspective);

	int32_t loadModelOBJ(const std::string &path, const std::string &file, const vec3f pos, const bool grouped, const bool addColliderGrid = false, const std::function<void(float)> showProgressCB = nullptr);
	int32_t loadSkybox(const std::string &path, const std::string &file, const std::function<void(float)> showProgressCB, const float scale = 1.f);
	
	void selectShader(const uint32_t ref) { currentShader = ref; resource->shaders[ref].Use(); resource->shaders[ref].setupShader(); }
	void setSun(const uint32_t col, const vec3f &pos, const int32_t ref = -1);
	void setFixedLight(const uint32_t col, const vec3f &pos, const int32_t ref = -1);
	void setFog(const uint32_t col, const float start, const float end, const int32_t ref = -1);

	void setPerspective3D(const int32_t width, const int32_t height, const float pspmul, const float znear, const float zfar);
	void setViewport(const Pi3Crecti &rect);
	void setOrthographic3D(const Pi3Crecti &rect, const float zoom, const float znear, const float zfar);
	void setViewport2D(const Pi3Crecti &rect, const float znear, const float zfar);
	void setup2Dprojection();
	void setup3Dprojection();
	void resize(const Pi3Crecti &rect);

	bool collide(const vec3f &pos, const vec3f &dir, const float radius) const;
	float collideFloor(const vec3f &pos) const;

	void addTexture(Pi3Cmodel &model, const std::string &txfile = "", const bool shaded = true);
	int32_t append3D(Pi3Cmodel &model, const std::string &txfile = "");
	int32_t append2D(Pi3Cmodel model, const std::string &txfile = "");

	int32_t addButton3D(Pi3Cmodel model, const std::string &fontfile, const std::string &text, const std::string &txtrfile);

	Pi3Cmodel * find(const std::string &name);

private:

	//Model matrics ...
	Pi3Cmatrix modelMatrix3D;				//scene 3D model matrix
	Pi3Cmatrix modelMatrix2D;				//scene 2D model matrix

	//Projection matrices ...
	Pi3Cmatrix projMatrix3D;
	Pi3Cmatrix projMatrix2D;

	std::string errorStr;
	int32_t currentShader = -1;
	uint32_t playerAvatarRef = 0;		//selects which avatar is the current players avatar

	float nearz2D = 0.1f;
	float farz2D = 500.f;
	float zoom = 1.f;
	float nearz3D = 1.0f;
	float farz3D = 5000.f;
	float perspective = PSPVALUE;

	Pi3Cmaterial selectMaterial;

	std::string getPathFile(std::string &filepath) const;
	void render(const float ticks, Pi3Cmatrix &projMatrix, Pi3Cmatrix &modelMatrix, std::vector<Pi3Cmodel> &models, Pi3Cmaterial *materialOverride);
	//void appendLight(Pi3Clight &light) { lights.push_back(light); }
	//void appendCamera(Pi3Ccamera &camera) { cameras.push_back(camera); }

};
