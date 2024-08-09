#include "Pi3Cutils.h"
#include "tessellate.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <string>

namespace Pi3Cutils {

	int32_t tesselatePath(std::vector<std::vector<float>> &contours, std::vector<float> &tris, std::vector<std::vector<float>> &edges)
	{

		//Find leftmost polygon (always an outside polygon)
		float leftx = 1e20f;
		int f = -1;
		for (size_t c = 0; c < contours.size(); c++) {
			auto& contour = contours[c];
			for (size_t p = 0; p < contour.size(); p += 2) {
				if (contour[p] < leftx) {
					leftx = contour[p];
					f = c;
				}
			}
		}

		//Work out CW/CCW of leftmost polygon by working out area of polygon ...
		std::vector<float>& contour = contours[f];
		float a = 0.f; size_t b = contour.size() - 2;
		for (size_t c = 0; c < contour.size() - 1; c += 2) {
			a += contour[b] * contour[c + 1] - contour[c] * contour[b + 1];
			b = c;
		}

		//reverse contours if inverted
		if (a < 0)
		{
			for (auto& contour: contours) {
				uint32_t vs = contour.size();
				for (size_t j = 0; j < (vs - 1) / 2; j += 2) {
					std::swap(contour[j], contour[vs - j - 2]);
					std::swap(contour[j + 1], contour[vs - j - 1]);
				}
			}
		}

		tessellate(&contours, NULL, &edges, true);	//Get outer edges
		tessellate(&contours, &tris, NULL, false);	//Get triangles
		int32_t triCount = tris.size() / 9;

		return triCount;	
	}
	
	void flipToOtherImage(uint8_t* src, uint8_t* dest, uint32_t w, uint32_t h)
	{
		uint32_t span = w * 4;
		uint8_t *pt = src + 0;
		uint8_t *pb = dest + (h - 1) * span;
		for (uint32_t y = 0; y < h; y++) {
			std::memcpy(pb, pt, span);
			pb -= span;
			pt += span;
		}
	}

	void flipImage(uint8_t* src, uint32_t w, uint32_t h)
	{
		//Destination can be the same as source...
		uint8_t* dest = src;
		uint32_t span = w * 4;
		uint8_t* pt = src + 0;
		uint8_t* pb = dest + (h - 1) * span;
		std::vector<uint8_t> tmp(span);

		for (uint32_t y = 0; y < h / 2; y++) {
			std::memcpy(&tmp[0], pb, span);
			std::memcpy(pb, pt, span);
			std::memcpy(pt, &tmp[0], span);
			pb -= span;
			pt += span;
		}
	}

	void saveBufferToPNG(const char* filename, std::vector<uint8_t>& snapShot, const int width, const int height)
	{
		std::vector<uint8_t> destimage;
		destimage.resize(snapShot.size());
		flipToOtherImage(&snapShot[0], &destimage[0], width, height);

		SDL_Surface* ss = SDL_CreateRGBSurfaceFrom(&destimage[0], width, height, 32, width * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_RWops* fo = SDL_RWFromFile(filename, "wb");
		IMG_SavePNG_RW(ss, fo, 0);
		SDL_RWclose(fo);
		SDL_FreeSurface(ss);
	}

	bool snapShot(const Pi3Crecti &rect, std::vector<uint8_t> &snapShot)
	{
		try {
			uint32_t size = (uint32_t)(rect.width*rect.height * 4);
			if (snapShot.size() < size) snapShot.resize(size);
			glViewport(rect.x, rect.y, rect.width, rect.height);
			std::vector<uint8_t> destimage;
			destimage.resize(snapShot.size());
			glReadPixels(rect.x, rect.y, rect.width, rect.height, GL_RGBA, GL_UNSIGNED_BYTE, &destimage[0]);
			flipToOtherImage(&destimage[0], &snapShot[0], rect.width, rect.height);
			//saveBufferToPNG("snapshot.png", snapShot, rect.width, rect.height);
			return true;
		}
		catch (const std::bad_alloc&) {
			return false;
		}
	}

	std::string ftostrdp(float n, int decimalPlaces)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(decimalPlaces) << n;
		return stream.str();
	}

	std::string endstr(std::string const& str, size_t const length) {
		if (length >= str.size()) return str;
		return str.substr(str.size() - length);
	}

	std::string filepath(const std::string& path, const std::string& filename, const std::string& ext)
	{
		const std::string gpath = (path.size() > 0 && *(path.end() - 1) != '/') ? path + "/" : path;
		return gpath + filename + ((ext[0] == '.') ? ext : "." + ext);
	}

	std::string extractPath(std::string& file)
	{
		std::size_t found = file.find_last_of("/\\");
		std::string path = file.substr(0, found);
		file = file.substr(found + 1);
		return path;
	}

	std::string extractName(const std::string& file)
	{
		std::size_t found = file.find_last_of("/\\");
		return file.substr(found + 1);
	}

	std::string lowercase(const std::string& str)
	{
		std::string lc = str;
		std::transform(lc.begin(), lc.end(), lc.begin(),
			[](unsigned char c) { return std::tolower(c); });
		return lc;
	}

	std::string getExt(const std::string& file)
	{
		std::size_t found = file.find_last_of(".");
		return lowercase(file.substr(found + 1));
	}

	float colToFloat(const uint32_t col) { 
		return (float)(col & 255) / 256.f + (float)((col >> 8) & 255) + (float)((col >> 16) & 255) * 256.f; 
	}

	void drawRect2D(vec3f pos, vec3f size, uint32_t colour)
	{
	 throw std::invalid_argument( "received negative value" );
	}

}
