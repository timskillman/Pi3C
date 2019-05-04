#pragma once

#include "Pi3Cmesh.h"
#include "Pi3Ctexture.h"
#include "Pi3Cmaterial.h"
#include "Pi3Cshader.h"
#include "Pi3Cshapes.h"
#include "Pi3CGL.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "Pi3Cfont.h"
#include "Pi3Csound.h"

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

class Pi3Cresource {
public:
	Pi3Cresource(uint32_t stride = 8)
	{ 
		this->stride = stride; 
		currentBuffer = -1;
		lastVBO = -1;
		startVBO = 0;
		createDefaultMaterial();
	}
	
	~Pi3Cresource()
	{

	}
	
	void appendMesh(Pi3Cmesh mesh) { meshes.push_back(mesh); }
	size_t meshCount() { return meshes.size(); }

	int32_t loadTexture(const std::string &path, const std::string &name, int32_t &texRef);  //loads a texture and returns a texture reference to the textures array (else -1 if failed)
	int32_t addTexture(const std::shared_ptr<Pi3Ctexture> &Texture, int32_t &texRef);
	void createDefaultMaterial(const std::string &name = "default");
	int32_t createDefaultTexture(int32_t &texID);
	Pi3Cmaterial * defaultMaterial() { return &materials[0]; }

	Pi3Ctexture* getTexture(int32_t ref) {
		if (ref >= 0 && ref <= (int32_t)textures.size()) return textures[ref].get(); else return nullptr;
	}

	int32_t addMesh(Pi3Cmesh &mesh, const bool deleteVerts = true, const bool dynamicBuffer = false, const uint32_t vertSize = 65535);		//returns mesh reference (or -1 if failed)
	
	void uploadMeshesToGPU();
	void updateGPUverts(const int VBO, const int vfrom, const int vsize, void * verts);
	void uploadGPUverts(const int bufref, const std::vector<float> &verts);
	void setRenderBuffer(const int bufRef);
	void renderMesh(const int meshRef, const GLenum rendermode = GL_TRIANGLES);
	int32_t touchMesh(const int meshRef, Pi3Ctouch &touch, const Pi3Cmatrix &tmat) const;
	
	int32_t addShader(const std::string &vertfile, const std::string &fragfile);
	void addFont(const char * path, const char * fontfile, int ptsize);
	void addMusic(const char * path, const char * musicfile);
	void addSound(const char * path, const char * soundfile);

	std::vector<float> * getLetterVerts();
	void uploadLetterVerts(uint32_t vertCount);

	std::shared_ptr<Pi3Cfont> findFont(const std::string &ffont) { return findResource(fonts, ffont); }
	std::shared_ptr<Pi3Cmusic> findMusic(const std::string &fmusic) { return findResource(music, fmusic); }
	std::shared_ptr<Pi3Csound> findSound(const std::string &sound) { return findResource(sounds, sound); }

	void clearAll();

	std::string error() { return errorStr; }

	inline size_t vertexBufferSize(uint32_t id) { return vertBuffer[id].size(); }
	
	std::vector<Pi3Cmesh> meshes;
	std::vector<std::vector<float>> vertBuffer;
	std::vector<Pi3Cmaterial> materials;
	std::vector<std::shared_ptr<Pi3Ctexture>> textures;
	std::vector<Pi3Cshader> shaders;
	std::map<std::string, std::shared_ptr<Pi3Cfont>> fonts;
	std::map<std::string, std::shared_ptr<Pi3Cmusic>> music;
	std::map<std::string, std::shared_ptr<Pi3Csound>> sounds;

	uint32_t vertCount;
	int32_t currentBuffer;
	uint32_t currentBufferSize;

	int calls;
	int32_t rectRef = -1;
	int32_t letterSheetRef = -1;

private:
	
	template <class T>
	T findResource(std::map<std::string, T> res, const std::string &sstr) {
		if (res.size() == 0) return T();
		auto fit = res.find(sstr);
		if (fit != res.end()) return fit->second;
		return T();
	}

	void addMeshVerts(const Pi3Cmesh &mesh, std::vector<float> &newverts, const uint32_t to, const uint32_t size, const uint32_t vertsize, const bool deleteVerts);
	GLuint VBOid[1000];
	uint32_t stride;
	int lastVBO;
	int startVBO;
	std::string errorStr;
};

