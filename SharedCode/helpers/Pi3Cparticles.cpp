#include "Pi3Cparticles.h"

/*  Very rough particle system for snow only 

Particles should eventally emit from a spherical area and move in a given direction and splay radius with a 'wander factor' and lifetime

*/

void Pi3Cparticles::create(Pi3Cresource* resources, const uint32_t count, const vec3f& pos, const ixyz& area, const uint32_t minsize, const uint32_t maxsize, const std::string &tex, uint32_t maxtypes)
{
	resource = resources;
	mesh.reset(new Pi3Cmesh);
	mesh->verts.reserve(count * mesh->stride * 6);

	startpos = pos;
	startsize = area;
	for (uint32_t r = 0; r<count; r++) {
		float flakesize = rand() % maxsize + minsize;
		vec2f fsize = vec2f(flakesize, flakesize);
		uint32_t type(rand() % 4);
		createParticle(pos, fsize, type, Pi3Cparticles::ixyz(1000, 0, 0));
	}

	model.meshRef = resource->addMesh(mesh.get());
	model.addTexture(resource, tex);
	model.material.illum = 1;
	model.material.alpha = .99f;
	mesh.reset();					//destroy the mesh in shapes and free up memory

}

void Pi3Cparticles::createParticle(const vec3f& pos, const vec2f size, const uint32_t type, const ixyz& wandr) {
	startpos = pos;
	position.push_back(pos + vec3f((startsize.x==0) ? 0 : (float)(rand() % startsize.x), (startsize.y==0) ? 0 : (float)(rand() % startsize.y), (startsize.z==0) ? 0 : (float)(rand() % startsize.z)));
	dir.push_back(vec3f(0, (float)-((rand() % 100000) + 10000), 0));
	wander.push_back(wandr);
	sizes.push_back(size);
	types.push_back(type);

	angle.push_back((float)(rand() % 628) / 100.f);
	angleinc.push_back((float)(rand() % 1000 - 500) * speed);

	mesh->addRect(pos, size, vec2f((float)(type)*0.25f, 0.f), vec2f(0.25f, 0.99f));
}

void Pi3Cparticles::updateStartArea(const ixyz& area)
{
	startsize = area;
}

void Pi3Cparticles::update(const size_t ref, const vec2f size, const uint32_t type)
{
	position[ref] = startpos + vec3f((startsize.x == 0) ? 0 : (float)(rand() % startsize.x), (startsize.y == 0) ? 0 : (float)(rand() % startsize.y), (startsize.z == 0) ? 0 : (float)(rand() % startsize.z));
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

void Pi3Cparticles::updateParticleCoordsRotated(const std::vector<vec3f>& pos, const std::vector<vec2f>& size, const std::vector<float>& angle)
{
	model.updateSpriteCoordsRotated(resource, pos, size, angle);
}