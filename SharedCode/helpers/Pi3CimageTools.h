#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>
#include "Pi3Ccontour.h"

namespace Pi3CimageTools {

	void drawLine(uint32_t* pic, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t col, uint32_t width);
	void floodfill(uint32_t* pic, int32_t x, int32_t y, uint32_t fill_colour, uint32_t width, uint32_t height, uint32_t* stack = nullptr, uint32_t stack_size = 0);
	void traceImage(uint32_t* pic, uint32_t width, uint32_t height, Pi3ClinPath &PathOut);
};
