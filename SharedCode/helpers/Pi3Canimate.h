#pragma once
#include "Pi3Cvector.h"
#include "Pi3Ccontour.h"

class Pi3CanimPath {
public:

	Pi3CanimPath();

	void append(const vec3f point, const vec3f& dir, const float period = 1.f, bool linear = true);
	vec3f calcPoint(int pathPoint, float div);
	float calcBezierLength(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4);
	void calcControls(const vec3f& p0, const vec3f& p1, const vec3f& p2, vec3f& c0, vec3f& c1);

	std::vector<vec3f> points;
	std::vector<vec3f> c1;
	std::vector<vec3f> c2;
	std::vector<float> periods;

private:
	float calcBezierAutoStep3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4);
	void createBezier3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, std::vector<vec3f>& points);
	vec3f calcBezierPoint3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, const vec3f& v4, float p);
	
};

class Pi3Canimate {
public:

	Pi3Canimate();

	void setSpin(const vec3f& spinAngles, const vec3f& spinRate);
	
	Pi3CanimPath animpath;

	bool enabled = false;
	bool repeat = false;
	bool reverse = false;
	float totalTime = 0;
	float currentTime = 0;
	float speed = 1.f;

private:
	int currentFrame = 0;
	float currentFract = 0.0;
};