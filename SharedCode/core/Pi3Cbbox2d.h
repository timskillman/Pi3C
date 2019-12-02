#pragma once

#include "Pi3Cvector.h"

class Pi3Cbbox2d {
public:

	Pi3Cbbox2d() {}

	Pi3Cbbox2d(vec2f _min, vec2f _max) {
		min = _min; max = _max;
	}

	vec2f min{ 1e8f, 1e8f };
	vec2f max{ -1e8f,-1e8f };

	void update(const vec2f& p);
	bool invalid() { return (min.x > max.x || min.y > max.y); }
	vec2f centre() { return vec2f((min.x + max.x) / 2.f, (min.y + max.y) / 2.f); }
	float width() { return max.x - min.x; }
	float height() { return max.y - min.y; }
	bool pointInside(const vec2f& p) { return (p.x > min.x && p.x<max.x && p.y>min.y && p.y < max.y); }
	bool pointOnEdge(const vec2f& p) { return (p.x == min.x || p.x == max.x || p.y == min.y || p.y == max.y); }

};