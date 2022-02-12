#include "Pi3Canimate.h"

Pi3CanimPath::Pi3CanimPath()
{

}

float Pi3CanimPath::calcBezierAutoStep3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4)
{
	vec3f p21 = v2 - v1;
	vec3f p32 = v3 - v2;
	vec3f p43 = v4 - v3;
	float astep = (float)sqrtf(p21.x * p21.x + p21.y * p21.y + p21.z * p21.z + p32.x * p32.x + p32.y * p32.y + p32.z * p32.z + p43.x * p43.x + p43.y * p43.y + p43.z * p43.z) * .5f;
	if (astep < 4) astep = 4; else if (astep > 24) astep = 24;
	return 1.f / astep;
}

vec3f Pi3CanimPath::calcBezierPoint3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, float p)
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

float Pi3CanimPath::calcBezierLength(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4)
{
	float vs = calcBezierAutoStep3d(v1, v2, v3, v4);
	float length = 0;
	vec3f lastpt = v1;

	for (float v = vs; v < 1.f; v += vs) {
		vec3f pt = calcBezierPoint3d(v1, v2, v3, v4, v);
		length += (pt - lastpt).length();
		lastpt = pt;
	}

	return length;
}

void Pi3CanimPath::createBezier3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, std::vector<vec3f>& points)
{
	float autoStep = calcBezierAutoStep3d(v1, v2, v3, v4);
	for (float i = autoStep; i < 0.99999f; i += autoStep) {
		points.push_back(calcBezierPoint3d(v1, v2, v3, v4, i));
	}
}

void Pi3CanimPath::append(const vec3f point, const vec3f& dir, const float period, bool linear)
{
	float per = period;
	if (points.size() > 0 && linear) {
		per = calcBezierLength(points.back(), points.back() + c1.back(), point - dir, point) * period;
	}

	points.push_back(point);
	c1.push_back(dir); //forward looking direction
	c2.push_back(-dir); //backward looking direction
	periods.push_back(per);
}

void Pi3CanimPath::calcControls(const vec3f& p0, const vec3f& p1, const vec3f& p2, vec3f& c0, vec3f& c1)
{
	//Magic number from https://www.tinaja.com/glib/ellipse4.pdf
	float a = sqrtf((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y) + (p1.z - p0.z) * (p1.z - p0.z));
	float b = sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
	const vec3f L = p2 - p0;
	float v1 = -0.55228475f * a / (b + a);
	float v2 = 0.55228475f * b / (b + a);
	c0 = vec3f(v1 * L.x, v1 * L.y, v1 * L.z);
	c1 = vec3f(v2 * L.x, v2 * L.y, v2 * L.z);
}

vec3f Pi3CanimPath::calcPoint(int pathPoint, float div)
{
	size_t sz = points.size();
	vec3f& p1 = points[pathPoint];
	vec3f& p2 = points[(pathPoint + 1) % sz];
	return calcBezierPoint3d(p1, p1+ c1[pathPoint], p2 + c2[pathPoint], p2, div);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Pi3Canimate::Pi3Canimate()
{

}

