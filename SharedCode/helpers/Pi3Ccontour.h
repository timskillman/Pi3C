#pragma once
#include "Pi3Cvector.h"
#include "Pi3Cbbox2d.h"
#include <vector>

class Pi3ClinContour
{
public:
	Pi3ClinContour() : hole(0), closed(false), parent(-1) {}
	~Pi3ClinContour() {}
	Pi3ClinContour(const std::vector<vec2f> &points);

	void addPoint(const vec2f& point);
	void addPoint(const float x, const float y) { addPoint(vec2f(x, y)); }
	void calcCurve(vec2f& p0, vec2f& p1, vec2f& p2, vec2f& c0, vec2f& c1);
	void createBezier(const vec2f& v1, const vec2f& v2, const vec2f& v3, const vec2f& v4, std::vector<vec2f>& points);
	void createBezier3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, std::vector<vec3f>& points);
	bool pointInside(vec2f& pt);
	void reverse();
	float area();

	std::vector<vec2f> points;
	Pi3Cbbox2d bbox;

	bool closed = false;
	int hole = 0;
	int parent = -1;
	vec2f leftmost;
};

class Pi3ClinPath {
public:

	Pi3ClinPath() {}

	std::vector<Pi3ClinContour> linContours;

	void append(const Pi3ClinContour& lcont);
	std::vector<std::vector<vec2f>> asVectors();
	std::vector<std::vector<float>> asFloats();

	Pi3Cbbox2d bbox;

};
