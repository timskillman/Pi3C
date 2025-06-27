#pragma once

#include <Pi3Cscene.h>
#include "bullets.h"

class Aliens {
public:

	enum action { AL_DESCENDING, AL_LANDED, AL_EXPLODING, AL_DESTINATION, AL_DEAD };

	void init(Pi3Cresource* resource, Pi3Cscene* scene, const uint32_t landref, const uint32_t ashipRef, const uint32_t maxAliens);
	void animate(Pi3Cresource* resource, Pi3Cscene* scene, const uint32_t landref, const float ticks);
	void hit(Pi3Cresource* resource, Pi3Cscene* scene, Bullets& bullets);

	std::vector<int32_t> alienrefs;
	std::vector<float> landHeight;
	std::vector<vec3f> dir;
	std::vector<action> actions;
	//std::vector<bool> landed;
	//std::vector<bool> arrived;
	std::vector<int32_t> exploding;

	uint32_t maxAliens = 0;
};

