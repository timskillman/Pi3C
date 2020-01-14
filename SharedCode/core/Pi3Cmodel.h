#pragma once

#include "Pi3Cresource.h"
#include "Pi3Cmatrix.h"
#include "Pi3Cshader.h"
#include "Pi3Cmaterial.h"
#include "Pi3Ccollision.h"
#include "Pi3Ctouch.h"
#include <functional>
#include <string>

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

//#define LODLUT_SIZE 256

//class Pi3CmodelBase {
//public:
//	Pi3CmodelBase() {}
//	~Pi3CmodelBase() {}
//
//	Pi3Cmatrix matrix;				//matrix used to transform mesh/group
//	int32_t meshRef;				//mesh reference to mesh in meshPool (-1 is no ref and not rendered)
//	Pi3Cmaterial material;			//copy of material in the meshPool - used for rendering mesh - can be modified
//	int32_t materialRef;			//material reference of original material in the meshPool
//	bool visible;					//visible in scene
//	bool deleted;					//deleted from scene
//	Pi3Cbbox3d bbox;
//};

class Pi3Cmodel {
public:

	enum modelFlags { SELECTED = 1, DELETED = 2, VISIBLE = 4, TOUCHABLE = 8, GRIDLOCK = 16, NORMALLOCK = 32 };

	//struct modelParams {
	//	std::string name;
	//	Pi3Cresource *resource = nullptr;
	//	Pi3Cmesh *mesh = nullptr;
	//	uint32_t diffuseColour = 0xffffffff;
	//	std::string path;
	//	std::string modelfile;
	//	std::string colliderfile;
	//	std::function<void(float)> showProgressCB = nullptr;
	//	bool asCollider = false;
	//	bool reserveBuffer = false;
	//	uint32_t bufferSize = 65535;
	//	bool deleteVerts = false;
	//	vec3f position = {};
	//	vec3f rotate = {};
	//};

	//Pi3Cmodel(const modelParams mp);
	Pi3Cmodel() {}
	Pi3Cmodel(Pi3Cresource *resource, Pi3Cmesh mesh, uint32_t diffuseColour = 0xffffffff);
	Pi3Cmodel(Pi3Cresource *resource, const std::string &name, Pi3Cmesh mesh, uint32_t diffuseColour = 0xffffffff); //add mesh with name
	Pi3Cmodel(Pi3Cresource *resource, const std::string &path, const  std::string &modelfile, const bool asCollider, std::function<void(float)> showProgressCB); //load model only
	Pi3Cmodel(Pi3Cresource *resource, const std::string &modelname, const std::string &path, const std::string &model, const std::string &collider, std::function<void(float)> showProgressCB = nullptr); //load model and collider
	~Pi3Cmodel() {}

// Functions

	void init();
	void create(Pi3Cresource *resource, Pi3Cmesh *mesh, uint32_t diffuseColour = 0xffffffff);
	void render(Pi3Cresource *resource, Pi3Cshader &shader, const Pi3Cmatrix *parent_matrix = nullptr, Pi3Cmaterial *materialOverride = nullptr);
	void renderBasic(Pi3Cresource *resource, Pi3Cshader &shader, const Pi3Cmatrix *parent_matrix = nullptr, Pi3Cmaterial *materialOverride = nullptr);
	void appendMesh(Pi3Cresource *resource, Pi3Cmesh mesh, bool asCollider);
	void updateMesh(Pi3Cresource* resource, const Pi3Cmesh& umesh);
	void loadOBJfile(Pi3Cresource *resource, std::string path, std::string modelfile, std::function<void(float)> showProgressCB, bool asCollider);
	bool collide(const Pi3Cresource *resource, const Pi3Cmatrix *parent_matrix, const vec3f &pos, const vec3f &dir, const float radius) const;
	float collideFloor(const Pi3Cresource *resource, const Pi3Cmatrix *parent_matrix, const vec3f &pos, float &prevHeight, const bool onehit = false) const;
	void loadModelAndCollider(Pi3Cresource *resource, std::string path, std::string model, std::string collider, std::function<void(float)> showProgressCB);
	void touch(const Pi3Cresource *resource, const Pi3Cmatrix *parent_matrix, Pi3Ctouch &touch, const int32_t level);
	int32_t addTexture(Pi3Cresource *resource, const std::string &txfile);
	int32_t addTexture(Pi3Cresource *resource, const std::shared_ptr<Pi3Ctexture> &texture);
	int32_t addPicture(Pi3Cresource *resource, const std::shared_ptr<Pi3Ctexture> &texture);
	void createRect2D(Pi3Cresource *resource, const vec2f &pos = vec2f(0, -1.f), const vec2f &size = vec2f(1.f, 1.f), const uint32_t colour = 0xffffffff);
	void textModel(Pi3Cresource *resource, Pi3Cfont *font, const std::string &text, const float wrapWidth);

	Pi3Cmodel * append(Pi3Cmodel model, vec3f offset = vec3f(0, 0, 0), vec3f rotation = vec3f(0,0,0));
	Pi3Cmodel * appendLOD(Pi3Cmodel model, float LODfrom, float LODtoo, vec3f offset = vec3f(0, 0, 0), vec3f rotation = vec3f(0, 0, 0));
	Pi3Cmodel * appendCollider(Pi3Cmodel &model, vec3f offset = vec3f(0, 0, 0), vec3f rotation = vec3f(0, 0, 0));

	void resize(vec3f size) { Pi3Cmatrix sm; sm.SetScales(size); matrix = matrix * sm; }
	void move(const vec3f &vec) { matrix.move(vec); }
	void rotate(const vec3f &rot) { matrix.rotate(rot); }

	void updateSpriteVerts(vertsPtr& vp, const uint32_t stride, const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec3f& p4);
	void updateSpriteBillboard(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const vec3f &lookat);
	void updateSpriteCoordsRotated(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const std::vector<float> &angles);
	void updateLineQuad(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec3f> &dir, const vec2f &size);

	Pi3Cmodel* find(const std::string &name); //searches model heirarchy and returns name if found (0 if not)

// Variables

	std::string name;				//model name (used for search)

	Pi3Cmatrix matrix;				//matrix used to transform mesh/group
	Pi3Cmaterial material;			//used for rendering mesh - can be modified
	vec3f rotation;

	uint32_t flags = 0;
	int32_t meshRef = -1;			//mesh reference to mesh in resources (-1 is no ref - nothing to render)
	int32_t materialRef = -1;		//material reference to original material in resource

	bool visible = true;			//visible in scene
	bool deleted = false;			//deleted from scene (useful for undo/redo)
	bool touchable = true;			//model is displayed but not touchable
	bool selected = false;			//model selected (useful for editing)
	bool asCollider = false;		//if true, then use this model as a collider as well as rendered

	int32_t choice = -1;			//if set to value 0 and above, this option will choose which model in the group to render - the rest are ignored

	Pi3Cbbox3d bbox;				//bounding box of transformed meshes that model holds

	float lodFrom = 0;				//LOD works by rendering the model Reference of the group if the eye distance is greater than the lodDistance
	float lodToo = 0;				//If the eye distance is less than lodDistance, then render the group contents instead.
									//If lodToo == 0, then dont do LOD checking
									//
									//Note: if more than one mesh is required for Level 0 LOD (ie. furthest distance), 
									//      then use the group as Level 0 LOD and make this lodDistance=0 and meshRef=-1

	std::vector<Pi3Cmodel> group;		//Group containing other models (or none at all)
	std::vector<Pi3Cmodel> colliders;	//Group containing colliders (or none at all)

private:

	bool collideSub(const Pi3Cresource *resource, const Pi3Cmatrix &pmat, const vec3f &pos, const vec3f &dir, const float radius) const;
	float collideFloorSub(const Pi3Cresource *resource, const Pi3Cmatrix &pmat, const vec3f &pos, float &prevHeight, const bool onehit) const;

};


