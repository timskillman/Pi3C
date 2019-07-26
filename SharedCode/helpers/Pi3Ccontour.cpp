#include "Pi3Ccontour.h"
#include "Pi3CbinStream.h"
#include "Pi3CimageTools.h"
#include "Pi3Cutils.h"

Pi3ClinContour::Pi3ClinContour(const std::vector<vec2f> &points) : closed(true), parent(-1), leftmost(1e20f, 0), hole(-1), points(points)
{

}

void Pi3ClinContour::addPoint(const vec2f& point) 
{
	points.push_back(point);
	if (point.x < leftmost.x) leftmost = point;
	bbox.update(point);
}

void Pi3ClinContour::calcCurve(vec2f& p0, vec2f& p1, vec2f& p2, vec2f& c0, vec2f& c1)
{
	float a = sqrtf((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y));
	float b = sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	const vec2f L = p2 - p0;
	float v1 = -0.5523f * a / (b + a);
	float v2 = 0.5523f * b / (b + a);
	c0 = vec2f(v1 * L.x, v1 * L.y);
	c1 = vec2f(v2 * L.x, v2 * L.y);
}


void Pi3ClinPath::append(const Pi3ClinContour& lcont)
{
	linContours.push_back(lcont);
	bbox.update(lcont.bbox.min);
	bbox.update(lcont.bbox.max);
}

void Pi3ClinPath::open(const char* filepath)
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

void Pi3ClinPath::drawToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc)
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

std::vector<std::vector<vec2f>> Pi3ClinPath::asVectors()
{
	std::vector<std::vector<vec2f>> contours;
	for (auto &cont : linContours) {
		contours.push_back(cont.points);
	}
	return contours;
}

std::vector<std::vector<float>> Pi3ClinPath::asFloats()
{
	std::vector<std::vector<float>> contours;
	for (auto& cont : linContours) {
		std::vector<float> points;
		for (auto& point : cont.points) {
			points.push_back(point.x);
			points.push_back(point.y);
		}
		contours.push_back(points);
	}
	return contours;
}

void Pi3ClinPath::fillToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc)
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
