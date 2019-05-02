#include "Pi3Ctexture.h"


Pi3Ctexture::Pi3Ctexture()
{
	init();
}

Pi3Ctexture::Pi3Ctexture(const char* file, bool _upload)
{
	init();
	loadFromFile(file);
	if (_upload) upload();
}

Pi3Ctexture::~Pi3Ctexture()
{
	if (textureID != 0) glDeleteTextures(1, &textureID);
	if (pixels) delete pixels;
}

void Pi3Ctexture::init()
{
	textureID = 0;
	pixels = nullptr;
	width = 0;
	height = 0;
	bpp = 0;
	format = GL_RGBA;
	pitch = 0;
	size = 0;
}

void Pi3Ctexture::createColour(uint32_t col)
{
	textureID = 0;
	width = 1;
	height = 1;
	pitch = 4;
	bpp = 4;
	size = 4;
	pixels = new uint8_t[size];
	memcpy(pixels, &col, size);
	format = GL_RGBA;

}

void Pi3Ctexture::create(uint32_t width, uint32_t height, uint32_t bpp)
{
	textureID = 0;
	this->width = width;
	this->height = height;
	this->bpp = bpp;
	pitch = width*bpp;
	size = height*pitch;
	pixels = new uint8_t[size];
	switch (bpp) {
	//case 1: format = GL_ALPHA8; break; //RPi doesnt understand this
	case 2: format = GL_RGB565; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}
}

void Pi3Ctexture::fromSurface(SDL_Surface* Surface)
{
	//Convert surface format to 8bit, RGBA 32bit ...
	SDL_Surface * convertedSurface = SDL_ConvertSurfaceFormat(Surface, SDL_PIXELFORMAT_RGBA32, 0);

	if (convertedSurface) {
		width =		convertedSurface->w;
		height =	convertedSurface->h;
		pitch =		convertedSurface->pitch;
		bpp =		convertedSurface->format->BytesPerPixel;
		size =		height * pitch;
		pixels =	new uint8_t[size];
		if (pixels) {
			memcpy(pixels, convertedSurface->pixels, size);
			format = GL_RGBA;
		}
		SDL_FreeSurface(convertedSurface);
	}
}

void Pi3Ctexture::fromSurfaceXY(SDL_Surface* Surface, const int x, const int y)
{
	//Convert surface format to 8bit, RGBA 32bit ...
	int yh = Surface->h; 
	if (yh+y >= height) yh = height-y;
	int xw = Surface->w;
	if (xw + x >= width) xw = width - x;
	int xp = xw * 4;
	for (int yy = 0; yy < yh; yy++) {
		uint8_t * tpix = &pixels[x + (yy+y) * pitch];
		uint8_t * spix = (uint8_t*)Surface->pixels;
		memcpy(tpix, &spix[yy * Surface->pitch], xp);
	}
}

void Pi3Ctexture::fromTextSurface(SDL_Surface* Surface)
{
	//Convert surface format to 8bit, RGBA 32bit ...

	if (Surface) {
		width = Surface->w;
		height = Surface->h;
		pitch = Surface->pitch;
		bpp = Surface->format->BytesPerPixel;
		size = height * pitch;
		pixels = new uint8_t[size];
		if (pixels) {
			memcpy(pixels, Surface->pixels, size);
			format = GL_RGBA;
		}
	}
}

void Pi3Ctexture::loadFromFile(const char* file)
{
	textureID = 0;

	SDL_Surface* Surface = IMG_Load(file);
	//if (Surface == NULL)
	//	SDL_ShowSimpleMessageBox(0, "Load image error", IMG_GetError(), NULL);
		
	if (Surface) {
		fromSurface(Surface);
		SDL_FreeSurface(Surface);
	}
}

void Pi3Ctexture::upload()
{
	if (textureID == 0 && pixels) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void Pi3Ctexture::changeTexels(uint8_t * texels, const GLint x, const GLint y, GLint w, GLint h) {
	// texels MUST be same format as texture format
	//if (textureID == 0) return;
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, (w==0) ? width : w, (h==0) ? height : h, format, GL_UNSIGNED_BYTE, texels);
}
