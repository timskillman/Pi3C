#include "Pi3Cparticles.h"

/*  Very rough particle system for snow only 

Particles should eventally emit from a spherical area and move in a given direction and splay radius with a 'wander factor' and lifetime

*/

void Pi3Cparticles::create(Pi3Cresource* resources, const uint32_t count, const vec3f& pos, const ixyz& area, const uint32_t minsize, const uint32_t maxsize, Pi3CspriteSheetInfo& spriteInfo, int32_t groupId)
{
	resource = resources;
	mesh.reset(new Pi3Cmesh);
	mesh->verts.reserve(count * mesh->stride * 6);

	startpos = pos;
	startsize = area;
	for (uint32_t r = 0; r<count; r++) {
		float particleSize = (float)(rand() % maxsize + minsize);
		vec2f fsize = vec2f(particleSize, particleSize);
		uint32_t tileRef = rand() % spriteInfo.frameCount;
		createParticle(pos, fsize, tileRef, spriteInfo, Pi3Cparticles::ixyz(1000, 0, 0));
	}

	model.meshRef = resource->addMesh(mesh.get(),groupId);
	model.addTexture(resource, spriteInfo.filename);
	model.material.illum = 1;
	model.material.alpha = .99f;
	mesh.reset();					//destroy the mesh in shapes and free up memory

}

void Pi3Cparticles::createParticle(const vec3f& pos, const vec2f size, const uint32_t tileRef, Pi3CspriteSheetInfo& spriteInfo, const ixyz& wandr) {
	startpos = pos;
	positions.push_back(pos + vec3f((startsize.x==0) ? 0 : (float)(rand() % startsize.x), (startsize.y==0) ? 0 : (float)(rand() % startsize.y), (startsize.z==0) ? 0 : (float)(rand() % startsize.z)));
	dir.push_back(vec3f(0, (float)-((rand() % 100000) + 10000), 0));
	wander.push_back(wandr);
	sizes.push_back(size);
	types.push_back(tileRef);

	angles.push_back((float)(rand() % 628) / 100.f);
	angleinc.push_back((float)(rand() % 1000 - 500) * speed);

	mesh->addRect(pos, size, spriteInfo.getTilePosition(tileRef), spriteInfo.getTileSize());
}

void Pi3Cparticles::updateStartArea(const ixyz& area)
{
	startsize = area;
}

void Pi3Cparticles::update(const size_t ref, const vec2f size, const uint32_t type)
{
	positions[ref] = startpos + vec3f((startsize.x == 0) ? 0 : (float)(rand() % startsize.x), (startsize.y == 0) ? 0 : (float)(rand() % startsize.y), (startsize.z == 0) ? 0 : (float)(rand() % startsize.z));
	dir[ref] = vec3f(0, (float)-((rand() % 100000) + 10000), 0);
	sizes[ref] = size;
	types[ref] = type;
	angles[ref] = (float)(rand() % 628) / 100.f;
	angleinc[ref] = (float)(rand() % 1000 - 500) * speed;
}

void Pi3Cparticles::animate(const float d)
{
	for (uint32_t i = 0; i < positions.size(); i++) {
		positions[i] += dir[i] * speed;
		if (wander[i].x) dir[i].x += (float)((rand() % wander[i].x) - wander[i].x / 2);
		if (wander[i].y) dir[i].y += (float)((rand() % wander[i].y) - wander[i].y / 2);
		if (wander[i].z) dir[i].z += (float)((rand() % wander[i].z) - wander[i].z / 2);
		angles[i] += angleinc[i];
		if (positions[i].y < d) 
			update(i, sizes[i], rand() % 4); // dead[i] = true;
	}
}

void Pi3Cparticles::updateParticles()
{
	model.updateSpriteCoordsRotated(resource, positions, sizes, angles);
}