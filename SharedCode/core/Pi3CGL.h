#pragma once

//Load cross-platform dependencies
#include "SDL_syswm.h"

#ifdef __WINDOWS__
#define GLEW_STATIC
#include "GL/glew.h"
#pragma comment (lib, "glew32s.lib")
#endif /* __WINDOWS__ (*** must come before SDL.h ***) */ 

#include <SDL.h>
#include <vector>
#include <string>

#ifdef __IPHONEOS__
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#endif /* __IPHONEOS__ */

#ifdef __WINDOWS__
#include <GL/gl.h>
#include <windows.h>
#elif defined (__LINUX__) && defined(__arm__)
#include <SDL_opengles.h>
//#include <SDL_opengl_glext.h>
#include <SDL_opengles2.h>
#include <SDL_opengles2_gl2ext.h>
#elif defined (__LINUX__) && defined(__i386__)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>
#endif

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

namespace Pi3CGL
{
	void showGLinfo();
	void log(std::string const &str);
};

