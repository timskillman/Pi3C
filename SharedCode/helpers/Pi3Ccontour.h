#pragma once
#include "Pi3Cvector.h"
#include "Pi3Cbbox2d.h"
#include "Pi3Ctexture.h"

#include <vector>
#include <memory>

class Pi3ClinContour
{
public:
	Pi3ClinContour() {}
	~Pi3ClinContour() {}
	Pi3ClinContour(const std::vector<vec2f> &points);

	void addPoint(const vec2f& point);
	void addPoint(const float x, const float y) { addPoint(vec2f(x, y)); }
	void calcCurve(vec2f& p0, vec2f& p1, vec2f& p2, vec2f& c0, vec2f& c1);

	std::vector<vec2f> points;
	Pi3Cbbox2d bbox;
	//int32 ref;
	bool closed;
	int hole;
	int parent;
	vec2f leftmost;
};

class Pi3ClinPath {
public:

	Pi3ClinPath() {}
	Pi3ClinPath(const char * file) { open(file); }

	std::vector<Pi3ClinContour> linContours;
	Pi3Cbbox2d bbox;

	void append(const Pi3ClinContour& lcont);
	void open(const char* filepath);
	void drawToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc);
	void fillToTexture(std::shared_ptr<Pi3Ctexture> tex, float sc);
	std::vector<std::vector<vec2f>> asVectors();
	std::vector<std::vector<float>> asFloats();
};
