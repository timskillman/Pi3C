#pragma once

#include <Pi3Cresource.h>
#include <Pi3Cmodel.h>
#include <vector>

class Pi3Cparticles {
public:

	struct ixyz { 
		ixyz() : x(0), y(0), z(0) {}
		ixyz(int x, int y, int z) : x(x), y(y), z(z) {}

		int x = 0; 
		int y = 0; 
		int z = 0; 
	};

	Pi3Cparticles() {

	}

	Pi3Cmodel create(Pi3Cresource* resource, const uint32_t count, const vec3f& pos, const ixyz& area, const uint32_t minsize, const uint32_t maxsize, const std::string &tex, const uint32_t maxtypes);
	void update(const size_t ref, const vec2f size, const uint32_t type);
	void createParticle(const vec3f& pos, const ixyz& area, const vec2f size, const uint32_t type, const ixyz& wandr);
	void animate(const float d);

	vec3f startpos;
	std::vector<vec3f> position;
	std::vector<vec2f> sizes;
	std::vector<float> angle;
	float speed = 0.0001f;

private:
	
	std::unique_ptr<Pi3Cmesh> mesh;
	std::vector<ixyz> startarea;
	std::vector<vec3f> dir;
	std::vector<ixyz> wander;
	std::vector<uint32_t> types;
	std::vector<uint32_t> life;
	std::vector<float> angleinc;
};

