#include "Pi3Cbbox2d.h"

void Pi3Cbbox2d::update(const vec2f& p) {
	if (p.x < min.x) min.x = p.x;
	if (p.x > max.x) max.x = p.x;
	if (p.y < min.y) min.y = p.y;
	if (p.y > max.y) max.y = p.y;
}
