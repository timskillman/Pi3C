#include "Pi3Cutils.h"
#include "tessellate.h"
#include <cstring>

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
	
}
