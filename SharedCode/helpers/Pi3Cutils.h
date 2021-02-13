#pragma once
#include "Pi3Cvector.h"
#include "Pi3Cwin.h"
#include "Pi3Crect.h"
#include <SDL_image.h>
#include <vector>

namespace Pi3Cutils
{
	int32_t tesselatePath(std::vector<std::vector<float>>& path, std::vector<float>& tris, std::vector<std::vector<float>>& edges);
	void flipImage(uint8_t* src, uint32_t w, uint32_t h);
	void flipToOtherImage(uint8_t* src, uint8_t* dest, uint32_t w, uint32_t h);
	bool snapShot(const Pi3Crecti &rect, std::vector<uint8_t> &snapShot);
	void saveBufferToPNG(const char* filename, std::vector<uint8_t>& snapShot, const int width, const int height);
	std::string ftostrdp(float n, int decimalPlaces);
	std::string endstr(std::string const& str, size_t const length);
	std::string filepath(const std::string& path, const std::string& filename, const std::string& ext);
	std::string extractPath(std::string& file);
	std::string extractName(const std::string& file);
	std::string getExt(const std::string& file);
	std::string lowercase(const std::string& str);
	float colToFloat(const uint32_t col);
	void drawRect2D(vec3f pos, vec3f size, uint32_t colour);


	template <typename I> std::string numToHexstr(I w, size_t hex_len = sizeof(I) << 1)
	{
		//Credit: AndreyS Scherbakov
		static const char* digits = "0123456789ABCDEF";
		std::string rc(hex_len, '0');
		for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
			rc[i] = digits[(w >> j) & 0x0f];
		return rc;
	}
}
