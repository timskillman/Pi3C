#include "Pi3Cparticles.h"

Pi3Cmodel Pi3Cparticles::create(Pi3Cresource* resource, const vec3f& pos, const ixyz& area, const uint32_t minsize, const uint32_t maxsize, const uint32_t maxtypes, const uint32_t count)
{
	mesh.reset(new Pi3Cmesh);
	mesh->verts.reserve(count * mesh->stride * 6);

	startpos = pos;
	for (uint32_t r = 0; r<count; r++) {
		float flakesize = rand() % maxsize + minsize;
		vec2f fsize = vec2f(flakesize, flakesize);
		uint32_t type(rand() % 4);
		createParticle(pos, area, fsize, type, Pi3Cparticles::ixyz(1000, 0, 0));
	}

	Pi3Cmodel model;
	model.meshRef = resource->addMesh(mesh.get());
	model.addTexture(resource, "../../Resources/models/maps/snowflakes4b.png");
	model.material.illum = 1;
	model.material.alpha = .99f;
	mesh.reset();					//destroy the mesh in shapes and free up memory

	return model;
}

void Pi3Cparticles::createParticle(const vec3f& pos, const ixyz& area, const vec2f size, const uint32_t type, const ixyz& wandr) {
	startpos = pos;
	startarea.push_back(area);
	position.push_back(pos + vec3f((area.x==0) ? 0 : (float)(rand() % area.x), (area.y==0) ? 0 : (float)(rand() % area.y), (area.z==0) ? 0 : (float)(rand() % area.z)));
	dir.push_back(vec3f(0, (float)-((rand() % 100000) + 10000), 0));
	wander.push_back(wandr);
	sizes.push_back(size);
	types.push_back(type);

	angle.push_back((float)(rand() % 628) / 100.f);
	angleinc.push_back((float)(rand() % 1000 - 500) * speed);

	mesh->addRect(pos, size, vec2f((float)(type)*0.25f, 0.f), vec2f(0.25f, 0.99f));
}

void Pi3Cparticles::update(const size_t ref, const vec2f size, const uint32_t type)
{
	position[ref] = startpos + vec3f((startarea[ref].x == 0) ? 0 : (float)(rand() % startarea[ref].x), (startarea[ref].y == 0) ? 0 : (float)(rand() % startarea[ref].y), (startarea[ref].z == 0) ? 0 : (float)(rand() % startarea[ref].z));
	dir[ref] = vec3f(0, (float)-((rand() % 100000) + 10000), 0);
	sizes[ref] = size;
	types[ref] = type;
	angle[ref] = (float)(rand() % 628) / 100.f;
	angleinc[ref] = (float)(rand() % 1000 - 500) * speed;
}

void Pi3Cparticles::animate(const float d)
{
	for (uint32_t i = 0; i < position.size(); i++) {
		position[i] += dir[i] * speed;
		if (wander[i].x) dir[i].x += (float)((rand() % wander[i].x) - wander[i].x / 2);
		if (wander[i].y) dir[i].y += (float)((rand() % wander[i].y) - wander[i].y / 2);
		if (wander[i].z) dir[i].z += (float)((rand() % wander[i].z) - wander[i].z / 2);
		angle[i] += angleinc[i];
		if (position[i].y < d) 
			update(i, sizes[i], rand() % 4); // dead[i] = true;
	}
}
