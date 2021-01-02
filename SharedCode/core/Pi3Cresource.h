#pragma once

#include "Pi3Cmesh.h"
#include "Pi3Ctexture.h"
#include "Pi3Cmaterial.h"
#include "Pi3Cshader.h"
#include "Pi3CGL.h"
#include "Pi3CvertsPtr.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "Pi3Cfont.h"
#include "Pi3Csound.h"

#define MAXVBO 1000

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

#define DEFAULT_STRIDE 9

struct Pi3CvertsBuffer {
	std::vector<float> verts;	//verts buffer
	uint32_t freePtr = 0;		//ptr to first free vert in verts buffer
	int32_t groupId = -1;		//set to system type
	int32_t bufferRef = -1;		//uploaded buffer reference

	int32_t free() { return verts.size() - freePtr; }
};

class Pi3Cresource {
public:
	Pi3Cresource() { }
	
	~Pi3Cresource() { }
	
	void init(const uint32_t stride = DEFAULT_STRIDE);
	//void appendMesh(Pi3Cmesh mesh) { meshes.push_back(mesh); }
	size_t meshCount() { return meshes.size(); }

	int32_t loadTexture(const std::string &path, const std::string &file, bool smooth = true);  //loads a texture and returns a texture reference to the textures array (else -1 if failed)
	int32_t addTexture(const std::shared_ptr<Pi3Ctexture> &Texture, bool smooth = true);
	int32_t getTextureID(int32_t texRef);
	void createDefaultMaterial(const std::string &name = "default");
	Pi3Cmesh createRect(const vec3f& pos, const vec2f& size, const uint32_t col, const vec2f& uvpos = vec2f(0,0), const vec2f& uvsize = vec2f(1.f,1.f));
	int32_t createDefaultTexture(int32_t &texID);
	Pi3Cmaterial * defaultMaterial() { return &materials[0]; }
	void deleteMaterialTexturesByID(int32_t groupId);
	void deleteMaterialsByID(int32_t groupId);
	void deleteVertsBuffer(int32_t groupId);
	void cleanTextures();	//removes empty textures

	Pi3Ctexture* getTexture(int32_t ref) {
		return (ref >= 0 && ref <= (int32_t)textures.size()) ? textures[ref].get() : nullptr;
	}

	//int32_t addMesh(Pi3Cmesh &mesh, const bool deleteVerts = true, const bool dynamicBuffer = false, const uint32_t vertSize = 2000000);		//returns mesh reference (or -1 if failed)
	int32_t addMesh(Pi3Cmesh * mesh, int32_t groupId, uint32_t maxsize = 500000);

	void addMeshOutlines(uint32_t meshref);

	vertsPtr getMeshVerts(uint32_t meshRef) {
		return vertsPtr(meshes[meshRef].vertOffset * meshes[meshRef].stride, &vertBuffer[meshes[meshRef].bufRef].verts);
	}

	//std::vector<float> * getMeshVerts(uint32_t meshRef, uint32_t &p) { 
	//	p = meshes[meshRef].vertOffset * meshes[meshRef].stride;
	//	return &vertBuffer[meshes[meshRef].bufRef];
	//}

	std::vector<float>* getMeshBuffer(uint32_t meshRef)
	{
		if (meshRef < 0) return nullptr;
		return &vertBuffer[meshes[meshRef].bufRef].verts;
	}

	//void uploadMeshesToGPU();
	void updateGPUverts(const int VBO, const int vfrom, const int vsize, const std::vector<float> &verts);
	//void uploadGPUverts(const int bufref, const std::vector<float> &verts);
	void updateMesh(uint32_t meshRef, uint32_t vertCount = 0, const uint32_t vertOffset = 0);
	void setRenderBuffer(const int bufRef, const uint32_t stride);
	void renderMesh(const int meshRef, const GLenum rendermode = GL_TRIANGLES);
	Pi3Crect renderText(const int meshRef, Pi3Cfont *font, const std::string &text, const vec3f &pos, const float wrapWidth, const uint32_t colour = 0xffffffff);
	int32_t touchMesh(const int meshRef, Pi3Ctouch &touch, const Pi3Cmatrix &tmat) const;
	
	int32_t addShader(const std::string &vertfile, const std::string &fragfile);
	void useBasicShader(const uint32_t shaderRef);
	void addFont(const char * path, const char * fontfile, int ptsize);
	std::shared_ptr<Pi3Cmusic> addMusic(const std::string& path, const std::string& musicfile);
	std::shared_ptr<Pi3Csound> addSound(const std::string& path, const std::string& soundfile);


	std::shared_ptr<Pi3Cfont> findFont(const std::string &ffont) { return findResource(fonts, ffont); }
	std::shared_ptr<Pi3Cmusic> findMusic(const std::string &fmusic) { return findResource(music, fmusic); }
	std::shared_ptr<Pi3Csound> findSound(const std::string &sound) { return findResource(sounds, sound); }

	void clearAll();

	std::string error() { return errorStr; }

	inline size_t vertexBufferSize(uint32_t id) { return vertBuffer[id].verts.size(); }
	
	std::vector<Pi3Cmesh> meshes;

	std::vector<Pi3CvertsBuffer> vertBuffer;

	//std::vector<std::vector<float>> vertBuffer;			//vertex buffers for modify and uploading to GPU
	//std::vector<uint32_t> vertBufferPtr;				//vertex buffer pointer for each vertex buffer (used for calculating free space in each buffer)

	std::vector<Pi3Cmaterial> materials;
	std::vector<std::shared_ptr<Pi3Ctexture>> textures;
	std::vector<Pi3Cshader> shaders;
	std::map<std::string, std::shared_ptr<Pi3Cfont>> fonts;
	std::map<std::string, std::shared_ptr<Pi3Cmusic>> music;
	std::map<std::string, std::shared_ptr<Pi3Csound>> sounds;

	uint32_t vertCount = 0;
	int32_t currentBuffer = -1;
	uint32_t currentBufferSize = 0;
	//uint32_t bufferPtr = 0;

	int calls;
	int32_t rectRef = -1;
	int32_t letterSheetRef = -1;
	int32_t lettersRef = -1;

private:
	
	template <class T>
	T findResource(std::map<std::string, T> res, const std::string &sstr) {
		if (res.size() == 0) return T();
		auto fit = res.find(sstr);
		if (fit != res.end()) return fit->second;
		return T();
	}

void addMeshVerts(const Pi3Cmesh &mesh, std::vector<float> &newverts, const uint32_t to, const uint32_t size, const uint32_t vertsize, const bool deleteVerts);

	GLuint VBOid[MAXVBO];
	//uint32_t stride;
	int lastVBO = -1;
	//int startVBO = 0;
	int currentVBO = 0;
	//bool uploaded = false;
	std::string errorStr;
};
