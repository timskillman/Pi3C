#pragma once

#include "Pi3Cvector.h"

class Pi3Cbbox2d {
public:

	vec2f min{ 1e8f,1e8f };
	vec2f max{ -1e8f,-1e8f };

	void update(const vec2f& p);
	vec2f centre() { return vec2f((min.x + max.x) / 2.f, (min.y + max.y) / 2.f); }
};