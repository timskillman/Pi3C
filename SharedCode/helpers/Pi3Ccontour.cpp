#include "Pi3Ccontour.h"

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
	//Magic number from https://www.tinaja.com/glib/ellipse4.pdf
	float a = sqrtf((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y));
	float b = sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	const vec2f L = p2 - p0;
	float v1 = -0.55228475f * a / (b + a);
	float v2 = 0.55228475f * b / (b + a);
	c0 = vec2f(v1 * L.x, v1 * L.y);
	c1 = vec2f(v2 * L.x, v2 * L.y);
}

bool Pi3ClinContour::pointInside(vec2f& point)
{
	//check point inside using right ray casting
	size_t i, j, vs = points.size();
	bool inside = false;

	for (i = 0, j = vs - 1; i < vs; j = i++) {
		if ( ((points[i].y >= point.y) != (points[j].y >= point.y)) &&
			(point.x <= (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x))
			inside = !inside;
	}

	return inside;
}

void Pi3ClinContour::reverse()
{
	uint32_t vs = points.size();
	for (size_t i = 0; i < (vs / 2); i++) {
		std::swap(points[i], points[vs - i - 1]);
	}
}

float Pi3ClinContour::area()
{
	float a = 0.f;
	uint32_t vs = points.size();
	size_t b = vs - 1;
	for (size_t c = 0; c < vs; c++) {
		a += points[b].x * points[c].y - points[c].x * points[b].y;
		b = c;
	}
	return a;
}

void Pi3ClinPath::append(const Pi3ClinContour& lcont)
{
	linContours.push_back(lcont);
	bbox.update(lcont.bbox.min);
	bbox.update(lcont.bbox.max);
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
