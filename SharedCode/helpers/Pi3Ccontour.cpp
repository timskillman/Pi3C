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

vec2f calcBezierPoint(const vec2f& v1, const vec2f& v2, const vec2f& v3, const vec2f& v4, float p)
{
	float ip = 1.f - p;
	float a0 = ip * ip * ip;
	float a1 = 3.f * p * ip * ip;
	float a2 = 3.f * p * p * ip;
	float a3 = p * p * p;
	return vec2f(a0 * v1.x + a1 * v2.x + a2 * v3.x + a3 * v4.x, a0 * v1.y + a1 * v2.y + a2 * v3.y + a3 * v4.y);
}

vec3f calcBezierPoint3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, float p)
{
	float ip = 1.f - p;
	float a0 = ip * ip * ip;
	float a1 = 3.f * p * ip * ip;
	float a2 = 3.f * p * p * ip;
	float a3 = p * p * p;
	return vec3f(a0 * v1.x + a1 * v2.x + a2 * v3.x + a3 * v4.x, 
				 a0 * v1.y + a1 * v2.y + a2 * v3.y + a3 * v4.y,
				 a0 * v1.z + a1 * v2.z + a2 * v3.z + a3 * v4.z);
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

float calcBezierAutoStep(const vec2f& v1, const vec2f& v2, const vec2f& v3, const vec2f& v4)
{
	vec2f p21 = v2 - v1;
	vec2f p32 = v3 - v2;
	vec2f p43 = v4 - v3;
	float astep = (float)sqrtf(p21.x * p21.x + p21.y * p21.y + p32.x * p32.x + p32.y * p32.y + p43.x * p43.x + p43.y * p43.y) * .5f;
	if (astep < 4) astep = 4; else if (astep > 24) astep = 24;
	return 1.f / astep;
}

void Pi3ClinContour::createBezier(const vec2f& v1, const vec2f& v2, const vec2f& v3, const vec2f& v4, std::vector<vec2f>& points)
{
	float autoStep = calcBezierAutoStep(v1, v2, v3, v4);
	for (float i = autoStep; i < 0.99999f; i += autoStep) {
		points.push_back(calcBezierPoint(v1, v2, v3, v4, i));
	}
}

float calcBezierAutoStep3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4)
{
	vec3f p21 = v2 - v1;
	vec3f p32 = v3 - v2;
	vec3f p43 = v4 - v3;
	float astep = (float)sqrtf(p21.x * p21.x + p21.y * p21.y + p21.z * p21.z + p32.x * p32.x + p32.y * p32.y + p32.z * p32.z + p43.x * p43.x + p43.y * p43.y + p43.z * p43.z) * .5f;
	if (astep < 4) astep = 4; else if (astep > 24) astep = 24;
	return 1.f / astep;
}

void Pi3ClinContour::createBezier3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, std::vector<vec3f>& points)
{
	float autoStep = calcBezierAutoStep3d(v1, v2, v3, v4);
	for (float i = autoStep; i < 0.99999f; i += autoStep) {
		points.push_back(calcBezierPoint3d(v1, v2, v3, v4, i));
	}
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
