#pragma once

#include <SDL.h>
#include <vector>
#include <string>
#include "Pi3Cmatrix.h"
#include "Pi3Cmaterial.h"
#include "Pi3Cvector.h"

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

class Pi3Cshader {
public:
	Pi3Cshader(const char *vertShaderFile, const char *fragShaderFile) : 
		program(-1), fogMinDist(50.f), fogMaxDist(500.f), fogColour(1.f, 1.f, 1.f)
	{
		sherror = create(vertShaderFile, fragShaderFile);
	}

	Pi3Cshader() : program(-1), fogMinDist(50.f), fogMaxDist(500.f), fogColour(1.f,1.f,1.f) {};
	~Pi3Cshader() {};

	std::string create(const char *vertShaderFile, const char *fragShaderFile);

	GLuint loadShaderFile(const GLenum eShaderType, const char *strShaderFile, std::string &error);
	GLuint loadShaderStr(const GLenum eShaderType, const std::string &shaderSource, std::string &error);
	bool CreateShaderProgram(const GLuint &vertexShader, const  GLuint &fragmentShader, std::string &error);

	inline void Use() { glUseProgram(program); }

	inline GLint GetUniformLocation(const char * name) { return glGetUniformLocation(program, name); }

	inline void Setf(const GLint ref, const float val) { if (ref >= 0) glUniform1f(ref, val); }

	inline void Seti(const GLint ref, const GLint val) { if (ref >= 0) glUniform1i(ref, val); }

	inline void Set2f(const GLint ref, const vec2f vec) { if (ref >= 0) glUniform2fv(ref, 1, &vec.x); }

	inline void Set3f(const GLint ref, const vec3f vec) { if (ref >= 0) glUniform3fv(ref, 1, &vec.x); }

	inline void Set4f(const GLint ref, const vec4f vec) { if (ref >= 0) glUniform4fv(ref, 1, &vec.x); }

	inline void SetMatrix4f(const GLint ref, const Pi3Cmatrix &matrix) { if (ref >= 0) glUniformMatrix4fv(ref, 1, GL_FALSE, matrix.get()); }

	inline GLint GetAttribLocation(const char * name) { return glGetAttribLocation(program, name); }
	
	inline GLint GetSetMatrix4f(const char * name, const Pi3Cmatrix &matrix) {
		GLint uniRef = glGetUniformLocation(program, name);
		if (uniRef>=0) glUniformMatrix4fv(uniRef, 1, GL_FALSE, matrix.get());
		return uniRef;
	}
	
	inline GLint GetSeti(const char * name, const GLint val) {
		GLint uniloc = glGetUniformLocation(program, name);
		if (uniloc >= 0) glUniform1i(uniloc, val);
		return uniloc;
	}
	
	inline GLint GetSetf(const char * name, const float val) {
		GLint uniloc = glGetUniformLocation(program, name);
		if (uniloc >= 0) glUniform1f(uniloc, val);
		return uniloc;
	}
	
	inline GLint GetSet2f(const char * name, const vec2f vec) {
		GLint uniloc = glGetUniformLocation(program, name);
		if (uniloc >= 0) glUniform2fv(uniloc, 1, &vec.x);
		return uniloc;
	}
	
	inline GLint GetSet3f(const char * name, const vec3f vec) {
		GLint uniloc = glGetUniformLocation(program, name);
		if (uniloc >= 0) glUniform3fv(uniloc, 1, &vec.x);
		return uniloc;
	}
	
	inline GLint GetSet4f(const char * name, const vec4f vec) {
		GLint uniloc = glGetUniformLocation(program, name);
		if (uniloc >= 0) glUniform4fv(uniloc, 1, &vec.x);
		return uniloc;
	}
	
	void setupShader();
	void setMaterial(Pi3Cmaterial &mat);
	void setColDiffuse(uint32_t colour);
	void setProjectionMatrix(const Pi3Cmatrix &matrix) { SetMatrix4f(perspectiveMatrixRef, matrix); }
	void SetModelMatrix(const Pi3Cmatrix &matrix) { SetMatrix4f(modelMatrixRef, matrix); }
	
	GLint activateTexture(GLuint texID, const char * uni, GLuint texActive);
	
	GLint setTexture(const char * uni, GLint texLoc);

	void setFog(float minDist, float maxDist, vec3f colour);
	void setSun(vec3f sundir, uint32_t colour);
	void setTicks(float ticks) { this->ticks = ticks; }
	std::string error() { return sherror; }

	float fogMinDist;
	float fogMaxDist;
	vec3f fogColour;
	vec3f lightPos;
	vec3f lightCol;

	float ticks = 1.f;

	Pi3Cmatrix ortho;
	Pi3Cmatrix persp;


private:

	GLint program;
	GLuint vertshader;
	GLuint fragshader;
	
	// Default Shader Refs for shader
	// ==============================
	//
	// These shader refs are passive and will 
	// be unset (-1) if not found in the shader

	GLint illuminationModelRef;
	GLint diffuseRef;
	GLint specularRef;
	GLint specularPower;
	GLint ambientRef;
	GLint emissiveRef;
	GLint texAnimRef;
	GLint textureRef;
	GLint reflectRef;

	GLint fogColourRef;
	GLint fogRangeRef;
	GLint fogMaxRef;

	GLint lightPosRef;
	GLint lightColRef;
	GLint perspectiveMatrixRef;
	GLint modelMatrixRef;
	
	bool init = false;
	std::string sherror;
	std::string lastMat;
};

