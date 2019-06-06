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
		fontSheetChar(const uint16_t c) : chr(c) {}

		int minx = 0;
		int maxx = 0;
		int miny = 0;
		int maxy = 0;
		int advance = 0;
		uint16_t chr = 0;
		Pi3Crect rect;
		Pi3Crecti recti;
		SDL_Rect sdlrect;
		uint32_t sheetRef = 0;
	};

	struct fontSheet {
		//~fontSheet() {  //for (auto &v : sheet) SDL_FreeSurface(v); }

		Pi3Cmaterial sheetMaterial;
		std::shared_ptr<Pi3Ctexture> sheet;
		//std::vector<SDL_Surface*> sheet;
		std::vector<fontSheetChar> chars;
	};

	enum textReaderType { RD_NONE, RD_HTML, RD_MARKDOWN };
	textReaderType textReader = RD_HTML;

	fontSheet fontsheet;

	TTF_Font * open(const char * path, const char * fontfile, int ptsize);
	std::shared_ptr<Pi3Ctexture> asTexture(const std::string &text, const uint32_t colour);

	void createFontSheet(const uint32_t sheetWidth, const uint32_t sheetHeight);
	void getStringSize(const std::string &text, int &w, int &h);
	Pi3Ctexture * textSurface(const std::string &text);
	uint32_t textureRects(std::string &text, std::vector<float> &verts, Pi3Crect &size, const float wrapWidth, const uint32_t stride);

	TTF_Font * font = nullptr;

private:
	void formatVerts(std::vector<float> &verts, const float wrapWidth, const float linex, const float maxHeight, const uint32_t p, const uint32_t linep, const uint32_t stride);
	void copyCharToSheet(fontSheetChar &ch, SDL_Surface* Surface, SDL_Surface* destSurface, int &lastX, int &lastY, int &maxHeight);

	SDL_Color colour;
	std::string name;
	SDL_Surface * wordsheet;
	Pi3CtextFormat format;
};