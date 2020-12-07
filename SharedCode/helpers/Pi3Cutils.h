#pragma once
#include "Pi3Cvector.h"
#include "Pi3Cwin.h"
#include "Pi3Crect.h"
#include <SDL_Image.h>
#include <vector>

namespace Pi3Cutils
{
	int32_t tesselatePath(std::vector<std::vector<float>>& path, std::vector<float>& tris, std::vector<std::vector<float>>& edges);
	void flipImage(std::vector<uint8_t> &src, std::vector<uint8_t> &dest, uint32_t w, uint32_t h);
	bool snapShot(const Pi3Crecti &rect, std::vector<uint8_t> &snapShot);
	void saveBufferToPNG(const char* filename, std::vector<uint8_t>& snapShot, const int width, const int height);
}
