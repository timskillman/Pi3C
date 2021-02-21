#pragma once

#include "SDL_ttf.h"
#include "Pi3Crect.h"
#include <string>
#include <memory>
#include "Pi3Ctexture.h"
#include "Pi3Cmaterial.h"
#include "Pi3Ctextformat.h"

class Pi3Cfont {
public:
	Pi3Cfont() : font(nullptr) {}
	Pi3Cfont(const char * path, const char * fontfile, int ptsize);
	~Pi3Cfont();

	struct fontSheetChar {

		fontSheetChar() {}
		fontSheetChar(const uint16_t c = 0) : chr(c) {}

		int minx = 0;
		int maxx = 0;
		int miny = 0;
		int maxy = 0;
		int advance = 0;
		uint16_t chr = 0;
		Pi3Crect rect = Pi3Crect();
		Pi3Crecti recti = Pi3Crecti();
		SDL_Rect sdlrect = SDL_Rect();
		uint32_t sheetRef = 0;
	};

	struct fontSheet {
		Pi3Cmaterial sheetMaterial;
		Pi3Ctexture sheet;
		std::vector<fontSheetChar> chars;
	};

	enum textReaderType { RD_NONE, RD_HTML, RD_MARKDOWN };

	TTF_Font * open(const char * path, const char * fontfile, int ptsize);
	Pi3Ctexture asTexture(const std::string &text, const uint32_t colour);
	void createFontSheet(const uint32_t sheetWidth, const uint32_t sheetHeight);
	void getStringSize(const std::string &text, int &w, int &h);
	Pi3Ctexture * textSurface(const std::string &text);
	uint32_t textureRects(const std::string &text, std::vector<float> &verts, Pi3Crect &size, const float wrapWidth, const uint32_t stride, const textReaderType textReader);

	fontSheet fontsheet;
	TTF_Font * font = nullptr;
	//textReaderType textReader = RD_HTML;

private:
	void formatVerts(std::vector<float> &verts, const float wrapWidth, const float linex, const float maxHeight, const uint32_t p, const uint32_t linep, const uint32_t stride);
	void copyCharToSheet(fontSheetChar &ch, SDL_Surface* Surface, SDL_Surface* destSurface, int &lastX, int &lastY, int &maxHeight);

	std::string name;
	Pi3CtextFormat format;
};