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
	if (uploaded) glDeleteTextures(1, &textureID);
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
	uploaded = false;
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

void Pi3Ctexture::create(uint32_t width, uint32_t height, uint32_t bytesPerPixel)
{
	textureID = 0;
	this->width = width;
	this->height = height;
	this->bpp = bytesPerPixel;
	pitch = width * bytesPerPixel;
	size = height * pitch;
	pixels = new uint8_t[size];
	switch (bytesPerPixel) {
	//case 1: format = GL_ALPHA8; break; //RPi doesnt understand this
	case 2: format = GL_RGB565; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	}
}

void rawBlit(uint8_t *src_pixels, const Pi3Crecti *src_rect, const uint32_t src_pitch, uint8_t *dest_pixels, const Pi3Crecti *dst_rect, const uint32_t dest_pitch)
{
	/*	A VERY RAW blit that does no bounds checking
		and ASSUMES the source rectangle is inside the
		source texture, the destination rectangle is 
		inside the destination texture and that both 
		rectangles are exactly the same width else she'll BLOW!
	*/

	uint32_t sint = sizeof(uint32_t);
	uint32_t dwidth = dst_rect->width * sint;								//Destination width (in bytes)
	uint32_t dp = dst_rect->y * dest_pitch + dst_rect->x * sint;		//Destination pointer start
	uint32_t sp = src_rect->y * src_pitch + src_rect->x * sint;					//Source pointer start

	for (int32_t y = 0; y < dst_rect->height; y++)
	{
		memcpy(&dest_pixels[dp], &src_pixels[sp], dwidth);
		dp += dest_pitch; sp += src_pitch;
	}
}

void Pi3Ctexture::blit(Pi3Crecti *src_rect, Pi3Ctexture *dest_tex, Pi3Crecti *dst_rect)
{
	rawBlit(pixels, src_rect, pitch, dest_tex->GetTexels(), dst_rect, dest_tex->GetPitch());
}

bool Pi3Ctexture::resize(const uint32_t new_width, const uint32_t new_height, const bool center)
{
	if (new_width > width || new_height > height) return false;

	//save existing content ...
	uint8_t * newpixels = new uint8_t(size);
	memcpy(&newpixels, pixels, size);
	
	//create new, resized pixel buffer ...
	if (pixels) delete pixels;
	uint32_t new_pitch = new_width * 4;
	uint32_t new_size = new_pitch * new_height;
	pixels = new uint8_t(new_size);
	
	//work out left or centred offsets ...
	uint32_t dx = (center) ? (new_width - width) / 2 : 0;
	uint32_t dy = (center) ? (new_height - height) / 2 : 0;

	//create source and destination rectangles ...
	Pi3Crecti src_rect(0, 0, width, height);
	Pi3Crecti dest_rect(dx, dy, width, height);

	//Blit saved content into new pixel buffer ...
	rawBlit(newpixels, &src_rect, pitch, pixels, &dest_rect, new_pitch);

	width = new_width;
	height = new_height;
	pitch = new_pitch;
	size = new_size;

	return true;
}

bool Pi3Ctexture::merge(Pi3Ctexture * mergetex, bool right_top)
{
	uint32_t new_width = width + mergetex->width;
	uint32_t new_height = (height > mergetex->height) ? height : mergetex->height;

	Pi3Crecti dest_rect(width, 0, mergetex->width, mergetex->height);
	Pi3Crecti src_rect(0, 0, mergetex->width, mergetex->height);

	if (resize(new_width, new_height, false))
	{
		rawBlit(mergetex->GetTexels(), &src_rect, mergetex->pitch, pixels, &dest_rect, pitch);
		return true;
	}
	return false;
}

void Pi3Ctexture::fromSurface(SDL_Surface* Surface)
{
	//Convert surface format to RGBA 32bit ...
	SDL_Surface * convertedSurface = SDL_ConvertSurfaceFormat(Surface, SDL_PIXELFORMAT_RGBA32, 0);
	if (convertedSurface) {
		fromTextSurface(convertedSurface);
		SDL_FreeSurface(convertedSurface);
	}
}

void Pi3Ctexture::fromTextSurface(SDL_Surface* Surface)
{
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

//void Pi3Ctexture::fromSurfaceXY(SDL_Surface* Surface, const int x, const int y)
//{
//	int yh = Surface->h;
//	if (yh + y >= height) yh = height - y;
//	int xw = Surface->w;
//	if (xw + x >= width) xw = width - x;
//	int xp = xw * 4;
//	for (int yy = 0; yy < yh; yy++) {
//		uint8_t * tpix = &pixels[x + (yy + y) * pitch];
//		uint8_t * spix = (uint8_t*)Surface->pixels;
//		memcpy(tpix, &spix[yy * Surface->pitch], xp);
//	}
//}

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

void Pi3Ctexture::saveAsPNG(const char* file)
{
	SDL_Surface *pngSurf = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	if (pngSurf) {
		memcpy(pngSurf->pixels, pixels, size);
		IMG_SavePNG(pngSurf, file);
		SDL_FreeSurface(pngSurf);
	}
}

void Pi3Ctexture::upload()
{
	if (!uploaded && pixels) {
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		uploaded = true;
	}
}

void Pi3Ctexture::changeTexels(uint8_t * texels, const GLint x, const GLint y, GLint w, GLint h) {
	// texels MUST be same format as texture format
	//if (textureID == 0) return;
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, (w==0) ? width : w, (h==0) ? height : h, format, GL_UNSIGNED_BYTE, texels);
}
