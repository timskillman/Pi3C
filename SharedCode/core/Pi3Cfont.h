#pragma once

#include "SDL_ttf.h"
#include "Pi3Crect.h"
#include <string>
#include <memory>
#include "Pi3Ctexture.h"

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
		SDL_Rect sdlrect;
		uint32_t sheetRef = 0;
	};

	struct fontSheet {
		~fontSheet() { for (auto &v : sheet) SDL_FreeSurface(v);  }

		std::vector<SDL_Surface*> sheet;
		std::vector<fontSheetChar> chars;
	};

	fontSheet fontsheet;

	TTF_Font * open(const char * path, const char * fontfile, int ptsize);
	std::shared_ptr<Pi3Ctexture> asTexture(const std::string &text, const uint32_t colour);

	void createFontSheet(const uint32_t sheetWidth, const uint32_t sheetHeight);
	void getStringSize(const std::string &text, int &w, int &h);
	SDL_Surface * textSurface(const std::string &text);

	TTF_Font * font = nullptr;

private:
	void copyCharToSheet(fontSheetChar &ch, SDL_Surface* Surface, SDL_Surface* destSurface, int &lastX, int &lastY, int &maxHeight);

	SDL_Color colour;
	std::string name;
	SDL_Surface * wordsheet;

};
