#pragma once
#include "Pi3Cscene.h"

class Bullets {
public:

	void init(Pi3Cresource* resource, Pi3Cscene* scene, const uint32_t maxBullets, const uint32_t bulletLife);
	void fire(Pi3Cscene* scene, const vec3f& pos, const vec3f& rot, const float flyspeed);
	void animate(Pi3Cscene* scene, const float ticks);
	bool hitCheck(Pi3Cscene* scene, const vec3f targetpos, const float targetRadius);

	std::vector<int32_t> bulletRefs;
	std::vector<vec3f> pos;
	std::vector<vec3f> dir;
	std::vector<int> life;

	uint32_t maxBullets = 100;
	uint32_t bulletLife = 300;
	int start = 0;
	int end = 0;
};
