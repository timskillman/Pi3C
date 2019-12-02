#pragma once
#include "Pi3CmemContour.h"
#include "Pi3CbinStream.h"
#include "Pi3CimageTools.h"
#include "Pi3Cutils.h"

void Pi3CmlinPath::open(const char* filepath)
{
	Pi3CbinStream bs(filepath);
	if (bs.is_open()) {
		size_t linco; bs.read(linco);
		for (size_t i = 0; i < linco; i++) {
			Pi3ClinContour lcont;
			bs.readContainer(lcont.points);
			if (lcont.points.size() > 0) {
				for (auto& point : lcont.points) lcont.bbox.update(point);
				append(lcont);
			}
		}
	}
}

void Pi3CmlinPath::drawToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc)
{
	uint32_t* pix = (uint32_t*)tex->GetTexels();
	uint32_t width = tex->GetWidth();

	for (auto& lcont : linContours) {
		vec2f lp = lcont.points[0] * sc;
		vec2f fp = lp;
		for (size_t i = 0; i < lcont.points.size(); i++) {
			vec2f p = lcont.points[i] * sc;
			Pi3CimageTools::drawLine(pix, lp.x, lp.y, p.x, p.y, 0xffffffff, width); //draw path in image
			lp = p;
		}
		Pi3CimageTools::drawLine(pix, lp.x, lp.y, fp.x, fp.y, 0xffffffff, width); //close path
	}
}

void Pi3CmlinPath::fillToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc)
{
	uint32_t* pix = (uint32_t*)tex->GetTexels();
	uint32_t width = tex->GetWidth();

	std::vector<std::vector<float>> contours;
	for (size_t i = 0; i < linContours.size(); i++) {
		contours.emplace_back();
		std::vector<vec2f>& lcont = linContours[i].points;
		std::vector<float>& contour = contours.back();
		contour.resize(linContours[i].points.size() * 2);
		memcpy(&contour[0], &lcont[0], lcont.size() * 2 * sizeof(float));
	}

	std::vector<float> tris;
	std::vector<std::vector<float>> edges;
	int triCount = Pi3Cutils::tesselatePath(contours, tris, edges);
}

