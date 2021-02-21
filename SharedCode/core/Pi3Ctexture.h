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
	Pi3Ctexture(const char* file, bool upload = true, bool smooth = true);
	Pi3Ctexture(const uint32_t width, const uint32_t height, const uint32_t bytesPerPixel) { create(width, height, bytesPerPixel); }
	Pi3Ctexture(const uint32_t width, const uint32_t height, std::vector<uint8_t>& img, const uint32_t bytesPerPixel) { fromImage(width, height, img, bytesPerPixel); }
	~Pi3Ctexture();

	bool loadFromFile(const char* file);
	void loadFromMemory(uint8_t* ptr, uint32_t size);
	void saveAsPNG(const char* file);
	void fromSurface(SDL_Surface* Surface);
	//void fromSurfaceXY(SDL_Surface* Surface, const int x, const int y);
	void convertSurface(SDL_Surface* Surface);
	void upload(bool smooth = true);
	void updateRect(uint8_t* pTexels, const GLint x = 0, const GLint y = 0, GLint w = 0, GLint h = 0); //update a rectangle of pixels in the texture
	void update();
	void Delete();
	void createColour(uint32_t col);
	void create(uint32_t width, uint32_t height, uint32_t bytesPerPixel);
	void fromImage(uint32_t width, uint32_t height, std::vector<uint8_t>& image, uint32_t bytesPerPixel);
	void flip();																				//flips texture on Y axis
	void shrinkPow2(const uint32_t pow2reduce);													//shrinks the texture by power of 2
	void pixel(uint32_t x, uint32_t y, uint32_t v) { pixels[x*bpp + y * pitch] = v; }
	//void rawBlit(uint8_t *src_pixels, const Pi3Crecti *src_rect, const uint32_t src_pitch, 
	//			uint8_t *dest_pixels, const Pi3Crecti *dst_rect, const uint32_t dest_pitch);	//Just blit the texture into another texture into rect position on the destination texture
	void blit(Pi3Crecti *src_rect, Pi3Ctexture *dest_tex, Pi3Crecti *dst_rect);				//Just blit the texture into another texture into rect position on the destination texture
	bool resize(const uint32_t new_width, const uint32_t new_height, const bool centre);
	void mapToTextureUnit(uint32_t unit);
	bool merge(Pi3Ctexture * mergetex, bool right_top);

	inline uint32_t GetWidth() const { return width; }
	inline uint32_t GetHeight() const { return height; }
	inline uint16_t GetDepth() const { return (uint16_t)bpp; }
	inline uint8_t * GetTexels() { return &pixels[0]; };
	inline uint32_t  GetPitch() const { return pitch; }
	inline bool isValid() const { return (pixels.size()>0); }
	GLuint textureID = 0;	//GL ID returned from upload (uint only)
	std::string name = "";

private:
	void init();
	
	std::vector<uint8_t> pixels;
	int bpp = 0;
	GLenum format = 0;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t pitch = 0;
	size_t size = 0;
	bool uploaded = false;
	
};
