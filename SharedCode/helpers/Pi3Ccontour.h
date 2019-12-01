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
	Pi3Cbbox2d bbox;

	void append(const Pi3ClinContour& lcont);
	std::vector<std::vector<vec2f>> asVectors();
	std::vector<std::vector<float>> asFloats();
};
