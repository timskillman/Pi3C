#include "Pi3Cutils.h"
#include "tessellate.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace Pi3Cutils {

	int32_t tesselatePath(std::vector<std::vector<float>> &contours, std::vector<float> &tris, std::vector<std::vector<float>> &edges)
	{

		//Find leftmost polygon (always an outside polygon)
		float leftx = 1e20f;
		int f = -1;
		for (size_t c = 0; c < contours.size(); c++) {
			std::vector<float>& contour = contours[c];
			for (size_t p = 0; p < contour.size(); p += 2) {
				if (contour[p] < leftx) {
					leftx = contour[p];
					f = c;
				}
			}
		}

		//Work out CW/CCW of leftmost polygon ...
		std::vector<float>& contour = contours[f];
		float a = 0.f; size_t p = contour.size() - 2;
		for (size_t q = 0; q < contour.size() - 1; q += 2) {
			a += contour[p] * contour[q + 1] - contour[q] * contour[p + 1];
			p = q;
		}

		//reverse contours if inverted
		if (a > 0)
		{
			for (size_t a = 0; a < contours.size(); a++) {
				std::vector<float>& contour = contours[a];
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
	
	void flipImage(std::vector<uint8_t> &src, std::vector<uint8_t> &dest, uint32_t w, uint32_t h)
	{
		uint32_t span = w * 4;
		uint8_t *pt = &src[0] + 0;
		uint8_t *pb = &dest[0] + (h - 1) * span;
		for (uint32_t y = 0; y < h; y++) {
			std::memcpy(pb, pt, span);
			pb -= span;
			pt += span;
		}
	}

	void saveBufferToPNG(const char* filename, std::vector<uint8_t>& snapShot, const int width, const int height)
	{
		std::vector<uint8_t> destimage;
		destimage.resize(snapShot.size());
		flipImage(snapShot, destimage, width, height);

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
			flipImage(destimage, snapShot, rect.width, rect.height);
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
}
