#include "Pi3Cfont.h"

Pi3Cfont::Pi3Cfont(const char * path, const char * fontfile, int ptsize) {
	if (!font) {
		font = open(path, fontfile, ptsize);
		createFontSheet(512, 512);
	}
}

Pi3Cfont::~Pi3Cfont() {
	TTF_CloseFont(font); font = nullptr;
}

TTF_Font * Pi3Cfont::open(const char * path, const char * fontfile, int ptsize) {
	name = fontfile;
	std::string file = (std::string(path) + std::string(fontfile));
	TTF_Font * openFont = TTF_OpenFont(file.c_str(), ptsize);
	return openFont;
}

std::shared_ptr<Pi3Ctexture> Pi3Cfont::asTexture(const std::string &text, const uint32_t colour)
{
	SDL_Surface* Surface = nullptr;
	if (text == "") Surface = SDL_CreateRGBSurface(0, 8, 8, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	else
		Surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDL_Color{ colour & 255, (colour >> 8) & 255, (colour >> 16) & 255 }, 300);
	if (Surface) {
		std::shared_ptr<Pi3Ctexture> ttex(new Pi3Ctexture);
		ttex->fromTextSurface(Surface);
		SDL_FreeSurface(Surface);
		return ttex;
	}
	return nullptr;
}

void Pi3Cfont::copyCharToSheet(fontSheetChar &ch, SDL_Surface* Surface, SDL_Surface* destSurface, int &lastX, int &lastY, int &maxHeight)
{
	int sheetWidth = destSurface->w;
	int sheetHeight = destSurface->h;
	int width = Surface->w;
	int height = Surface->h;
	if (height > maxHeight) maxHeight = height;

	//check if character image will fit in sheet and wrap / create new sheet if necessary ...
	if (lastX + width > sheetWidth) {
		lastX = 0;
		if (lastY + maxHeight > sheetHeight) {
			uint32_t *pt = (uint32_t *)destSurface->pixels;
			for (int32_t p = 0; p < destSurface->w*destSurface->h; p++) pt[p] = 0xffffff | (pt[p] & 0xff000000);
			fontsheet.sheet.push_back(destSurface);
			lastY = 0; maxHeight = height;
		} 
		else 
			lastY += maxHeight;
	}

	//positions of character image on sheet ...
	ch.rect = Pi3Crect((float)lastX / sheetWidth, (float)lastY / sheetHeight, (float)width / sheetWidth, (float)height / sheetHeight);
	ch.sdlrect.x = lastX; ch.sdlrect.y = lastY; ch.sdlrect.w = width; ch.sdlrect.h = height;

	//blit the character image to the sheet ...
	SDL_BlitSurface(Surface, NULL, destSurface, &ch.sdlrect);

	lastX += width;
	ch.sheetRef = fontsheet.sheet.size();
}

void Pi3Cfont::createFontSheet(const uint32_t sheetWidth, const uint32_t sheetHeight)
{
	/* Create a font sheet with glpyh metrics */
	char c[2] = { 0,0 };
	int lastX = 0; int lastY = 0; int maxHeight = 0;
	SDL_Color white{ 255, 255, 255 };

	SDL_Surface *destSurface = SDL_CreateRGBSurface(0, sheetWidth, sheetHeight, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	
	for (uint16_t cc = 0; cc < 255; cc++) {
		c[0] = ((cc==0) ? 1 : cc) & 255; //char 0 wont compute!
		SDL_Surface * chSurface = TTF_RenderText_Blended(font, c, white);
		if (chSurface) {
			fontSheetChar ch(cc);
			if (TTF_GlyphMetrics(font, cc, &ch.minx, &ch.maxx, &ch.miny, &ch.maxy, &ch.advance) != -1) {
				copyCharToSheet(ch, chSurface, destSurface, lastX, lastY, maxHeight);
				fontsheet.chars.push_back(ch);
			}
			SDL_FreeSurface(chSurface);
		}
	}

	uint32_t *pt = (uint32_t *)destSurface->pixels;
	for (int32_t p = 0; p < destSurface->w*destSurface->h; p++) pt[p] = 0xffffff | (pt[p] & 0xff000000);
	fontsheet.sheet.push_back(destSurface);

	SDL_RWops *fo = SDL_RWFromFile("fontsheet.png", "wb");
	IMG_SavePNG_RW(fontsheet.sheet[0], fo, 0);
	SDL_RWclose(fo);
}

void Pi3Cfont::getStringSize(const std::string &text, int &w, int &h)
{
	w = 0; h = 0;
	for (auto &c : text) {
		fontSheetChar &ch = fontsheet.chars[(uint8_t)c];
		w += ch.sdlrect.w;
		if (ch.sdlrect.h > h) h = ch.sdlrect.h;
	}
}

SDL_Surface * Pi3Cfont::textSurface(const std::string &text)
{
	int w, h;
	getStringSize(text, w, h);
	SDL_Surface *chSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	SDL_Rect rect;
	rect.x = 0; rect.y = 0;
	for (auto &c : text) {
		fontSheetChar &ch = fontsheet.chars[(uint8_t)c];
		rect.w = ch.sdlrect.w; rect.h = ch.sdlrect.h;
		SDL_BlitSurface(fontsheet.sheet[ch.sheetRef], &ch.sdlrect, chSurface, &rect);
		rect.x += rect.w;
	}
	return chSurface;
}

