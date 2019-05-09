#pragma once

#include "Pi3CGL.h"
#include "Pi3Crect.h"
#include <SDL_image.h>
#include <stdint.h>

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

class Pi3Ctexture
{
public:
	Pi3Ctexture();
	Pi3Ctexture(const char* file, bool upload = true);
	Pi3Ctexture(const uint32_t width, const uint32_t height, const uint32_t bpp) { create(width, height, bpp); }
	~Pi3Ctexture();

	void loadFromFile(const char* file);
	void saveAsPNG(const char* file);
	void fromSurface(SDL_Surface* Surface);
	//void fromSurfaceXY(SDL_Surface* Surface, const int x, const int y);
	void fromTextSurface(SDL_Surface* Surface);
	void upload();
	void changeTexels(uint8_t * pTexels, const GLint x = 0, const GLint y = 0, GLint w = 0, GLint h = 0);
	void createColour(uint32_t col);
	void create(uint32_t width, uint32_t height, uint32_t bpp);
	void pixel(uint32_t x, uint32_t y, uint32_t v) { pixels[x*bpp + y * pitch] = v; }
	void rawBlit(Pi3Crecti *src_rect, Pi3Ctexture *dest_tex, Pi3Crecti *dest_rect);								//Just blit the texture into another texture into rect position on the destination texture

	inline uint32_t GetWidth() const { return width; }
	inline uint32_t GetHeight() const { return height; }
	inline uint16_t GetDepth() const { return (uint16_t)bpp; }
	inline uint8_t * GetTexels() { return pixels; };
	inline uint32_t  GetPitch() const { return pitch; }
	inline bool isValid() { return (pixels != nullptr); }
	GLuint textureID = 0;	//GL ID returned from upload (uint only)
	
private:
	void init();
	
	uint8_t * pixels = nullptr;
	int bpp = 0;
	GLenum format = 0;
	int width = 0;
	int height = 0;
	int pitch = 0;
	size_t size = 0;
	bool uploaded = false;
};
