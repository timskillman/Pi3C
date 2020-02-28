#pragma once

#include <Pi3Cresource.h>
#include <Pi3Cmodel.h>
#include "Pi3Csprite.h"
#include <vector>

class Pi3Cparticles {
public:

	struct ixyz { 
		int x = 0;
		int y = 0;
		int z = 0;

		ixyz() : x(0), y(0), z(0) {}
		ixyz(int x, int y, int z) : x(x), y(y), z(z) {}
	};

	Pi3Cparticles() {

	}

	void create(Pi3Cresource* resource, const uint32_t count, const vec3f& pos, const ixyz& area, const uint32_t minsize, const uint32_t maxsize, Pi3CspriteSheetInfo &spriteAni);
	void update(const size_t ref, const vec2f size, const uint32_t type);
	void createParticle(const vec3f& pos, const vec2f size, const uint32_t tileRef, Pi3CspriteSheetInfo& spriteAni, const ixyz& wandr);
	void animate(const float d);
	void updateParticleCoordsRotated(const std::vector<vec3f>& pos, const std::vector<vec2f>& size, const std::vector<float>& angle);
	void updateStartArea(const ixyz& area);

	vec3f startpos;
	std::vector<vec3f> position;
	std::vector<vec2f> sizes;
	std::vector<float> angle;
	ixyz startsize;
	float speed = 0.0001f;
	Pi3Cmodel model;

private:
	
	std::unique_ptr<Pi3Cmesh> mesh;
	
	std::vector<vec3f> dir;
	std::vector<ixyz> wander;
	std::vector<uint32_t> types;
	std::vector<uint32_t> life;
	std::vector<float> angleinc;
	Pi3Cresource* resource = nullptr;
	
};

