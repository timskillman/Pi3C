#include "Pi3Cfont.h"
#include "Pi3Ctextformat.h"

#define READHTML
#ifdef READHTML
#include "Pi3Chtml.h"
#endif

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
		Surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), SDL_Color{ (uint8_t)(colour & 255), (uint8_t)((colour >> 8) & 255), (uint8_t)((colour >> 16) & 255) }, 300);
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
			/*   This should not occur as we can't use multiple font images with a single mesh ... just raise error and make sheet big enough

			uint32_t *pt = (uint32_t *)destSurface->pixels;
			for (int32_t p = 0; p < destSurface->w*destSurface->h; p++) pt[p] = 0xffffff | (pt[p] & 0xff000000);
			fontsheet.sheet.push_back(destSurface);
			lastY = 0; maxHeight = height;
			*/
		} 
		else 
			lastY += maxHeight;
	}

	//positions of character image on sheet ...
	ch.rect = Pi3Crect((float)lastX / sheetWidth, (float)(lastY+height) / sheetHeight, (float)width / sheetWidth, (float)height / sheetHeight);
	ch.sdlrect.x = lastX; ch.sdlrect.y = lastY; ch.sdlrect.w = width; ch.sdlrect.h = height;
	ch.recti.x = lastX; ch.recti.y = lastY; ch.recti.width = width; ch.recti.height = height;

	//blit the character image to the sheet ...
	SDL_BlitSurface(Surface, NULL, destSurface, &ch.sdlrect);

	lastX += width+1;	// add 1 for clean look
	ch.sheetRef = 0;	//fontsheet.sheet.size();
}

void Pi3Cfont::createFontSheet(const uint32_t sheetWidth, const uint32_t sheetHeight)
{
	/* Create a font sheet with glpyh metrics 0-255 range only for now */
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

	//make rgb surface white (or could be a colour) and retain alpha ...
	uint32_t *pt = (uint32_t *)destSurface->pixels;
	for (int32_t p = 0; p < destSurface->w*destSurface->h; p++) pt[p] = 0xffffff | (pt[p] & 0xff000000);

	//Create texture and material ...
	fontsheet.sheet.reset(new Pi3Ctexture());
	fontsheet.sheet->fromTextSurface(destSurface);
	fontsheet.sheet->upload(); //upload to GPU
	fontsheet.sheetMaterial.texID = fontsheet.sheet->textureID;
	fontsheet.sheetMaterial.texRef = 0; //need to validate rendering texture
	fontsheet.sheetMaterial.SetColDiffuse(0xffffffff);
	//fontsheet.sheetMaterial.SetColAmbient(0xffffffff);
	fontsheet.sheetMaterial.alpha = 0.99f;
	fontsheet.sheetMaterial.illum = 1; //non shaded illuminat

	//fontsheet.sheet->saveAsPNG("textureSheet.png"); //debug
	SDL_FreeSurface(destSurface);
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

Pi3Ctexture * Pi3Cfont::textSurface(const std::string &text)
{
	int w, h;
	getStringSize(text, w, h);
	Pi3Ctexture *chtex = new Pi3Ctexture();
	chtex->create(w, h, 4);
	Pi3Crecti drect;
	drect.x = 0; drect.y = 0;

	for (auto &c : text) {
		fontSheetChar &ch = fontsheet.chars[(uint8_t)c];
		drect.width = ch.recti.width; drect.height = ch.recti.height;
		//fontsheet.sheet->rawBlit2(&ch.recti, chtex, &drect);
		fontsheet.sheet->blit(&ch.recti, chtex, &drect);
		drect.x += drect.width;
	}
	return chtex;
}

void Pi3Cfont::formatVerts(std::vector<float> &verts, const float wrapWidth, const float linex, const float maxHeight, const uint32_t p, const uint32_t linep, const uint32_t stride)
{
	//uint32_t stride = 9;
	uint32_t charsize = 6 * stride;
	float xo = 0; float sxo = 0;
	switch (format.justify) {
	case Pi3CtextFormat::LEFT_JUSTIFY: break;
	case Pi3CtextFormat::CENTER_JUSTIFY: xo = (wrapWidth - linex) / 2.f; break;
	case Pi3CtextFormat::RIGHT_JUSTIFY: xo = (wrapWidth - linex); break;
	case Pi3CtextFormat::FULL_JUSTIFY: 
		if (linex/ wrapWidth > 0.7f) sxo = (wrapWidth - linex)/ (float)((p-linep)/charsize);
		break;
	}

	for (uint32_t pp = linep; pp < p; pp += charsize) {
		float ht = (maxHeight - (verts[pp + 1 + stride] - verts[pp + 1])); // -maxHeight;
		for (uint32_t k = 0; k < 6; k++) {
			verts[pp + k * stride] += xo;
			verts[pp + 1 + k * stride] -= ht;
		}
		xo += sxo;
	} 
	if (format.endJustify) { format.justify = Pi3CtextFormat::LEFT_JUSTIFY; format.endJustify = false; }
}

#define CreateVerts(x,y,ux,uy)										\
		verts[p] = x; verts[p + 1] = y; verts[p + 2] = -10.f;		\
		verts[p + 3] = 0; verts[p + 4] = -1.f; verts[p + 5] = 0;	\
		verts[p + 6] = ux; verts[p + 7] = 1.f-uy;					\
		p += stride;												\


uint32_t Pi3Cfont::textureRects(const std::string &text, std::vector<float> &verts, Pi3Crect &size, const float wrapWidth, const uint32_t stride, const textReaderType textReader)
{
	/* Creates a letter sheet made of rectangles with UV mapping to render a font sheet
	   Fun stuff could be done here such as warping the letter geometry etc.. */

	uint32_t p = 0;
	//uint32_t stride = 9;
	uint32_t tsize = text.size();
	uint32_t spacep = 0;
	uint32_t spacei = 0;
	uint32_t linep = 0;
	float linex = 0;
	//uint32_t charsize = 6 * stride; //size of character rectangle
	format.justify = Pi3CtextFormat::FULL_JUSTIFY;
	float x = 0; float y = 0; 
	float maxHeight = 0; float maxWidth = 0;
	uint32_t i = 0;

	while (i<tsize) {

		char c = text[i];
		if (c == 10) {
			formatVerts(verts, wrapWidth, x, maxHeight, p, linep, stride);
			linep = p;
			x = 0; y -= maxHeight; maxHeight = 0; linex = x;
		}
		else
		{
			switch (textReader) {
			case RD_NONE: break;
#ifdef READHTML
			case RD_HTML: Pi3Chtml::readHTML(c, text, i, format); break;
#endif
			case RD_MARKDOWN: break;
			}

			if (c > 0) {
				fontSheetChar &ch = fontsheet.chars[(uint8_t)c];
				float xw = x + (float)ch.sdlrect.w * format.scale;
				float h = (float)ch.sdlrect.h * format.scale;
				float ux = ch.rect.x; float uy = ch.rect.y;
				float uw = ch.rect.width; float uh = ch.rect.height;

				if (h > maxHeight) maxHeight = h;

				if (xw > wrapWidth) { 
					while (text[i++] == ' '); //ignore spaces
					p = spacep; //wind back and overwrite word that overhangs wrapWidth
					i = spacei; //point to beginning of word in text string
					formatVerts(verts, wrapWidth, linex, maxHeight, p, linep, stride);
					x = 0; y -= maxHeight; maxHeight = 0; linex = x; linep = p;
				}
				else {
					float yo = (format.scaleYoffset > 0.f) ? y+h * format.scaleYoffset : y;
					CreateVerts(x, yo-h, ux, uy);
					CreateVerts(xw, yo - h, ux + uw, uy);
					CreateVerts(xw + format.italicSlant, yo, ux + uw, uy + uh);
					CreateVerts(x, yo-h, ux, uy);
					CreateVerts(xw + format.italicSlant, yo, ux + uw, uy + uh);
					CreateVerts(x + format.italicSlant, yo, ux, uy + uh);
					x = xw;
					if (x > maxWidth) maxWidth = x;

					if (c == ' ') { //keep note of last space on line in case we word wrap
						spacep = p;
						spacei = i;
						linex = x;
					}
				}
			}


		}
		i++;
	}
	size.x = 0; size.y = 0;
	size.width = maxWidth; size.height = y+ maxHeight;
	return p;	//return number of floats created
}
