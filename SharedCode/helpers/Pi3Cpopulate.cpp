#include "Pi3Cpopulate.h"



Pi3Cmodel Pi3Cpopulate::Populate(Pi3Cresource* resource, const uint32_t maxShapes, Pi3Cmodel& model, const std::string& texturePath, uint32_t maxTextures, const float radius)
{
	shapes.init(maxTextures);

	uint32_t tcount = 0;
	uint32_t type = 0;

	Pi3Cbbox3d& lbox = model.bbox;
	float toph = lbox.max.y;
	int lw = lbox.width() - radius;
	int ld = lbox.depth() - radius;
	float lxc = lbox.center().x;
	float lzc = lbox.center().z;
	float clearCenter = 1800.f;
	shapes.mesh->verts.reserve(maxShapes * 9 * 12);
	int rad = (int)radius;

	while (tcount < maxShapes) {

		vec3f clusterCentre((float)(rand() % lw - lw / 2) + lxc, 0, (float)(rand() % ld - ld / 2) + lzc);	//Cluster centre ...
		while (clusterCentre.x > -(clearCenter + 500.f) && clusterCentre.x<(clearCenter + 500.f) && clusterCentre.z>-clearCenter && clusterCentre.z < clearCenter) 
			clusterCentre = vec3f((float)(rand() % lw - lw / 2) + lxc, 0, (float)(rand() % ld - ld / 2) + lzc);
		uint32_t clusterCount = tcount + rand() % 100 + 20;										//count in this cluster

		while (tcount < maxShapes && tcount < clusterCount) {

			float rsize = (float)(rand() % 40) + 10.f * (float)(type + 2);						//random shape size 10-50
			float radius = (float)(rand() % rad);												//radius
			float angle = rnd(360) / PI2;														//random angle 0-360
			vec3f pos = clusterCentre + vec3f(cos(angle) * radius, toph, sin(angle) * radius);	//calc shape position 

			float ht = 1e8f;  //start from way up high!
			ht = model.collideFloor(resource, nullptr, -pos, ht, true);
			if (ht < 1e8f) {
				pos.y = toph - ht + rsize;
				vec2f size(rsize, rsize);
				shapes.create(pos, size, type);
				tcount++;
			}
		}
		type = (type + 1) % maxTextures;	//simply cycle through tree types per cluster
	}

	Pi3Cmodel popmodel;
	popmodel.meshRef = resource->addMesh(shapes.mesh.get());
	popmodel.addTexture(resource, texturePath);
	popmodel.material.illum = 0;
	popmodel.material.alpha = 1.f;
	popmodel.material.cullface = false;		//show both sides of polygons

	shapes.mesh.reset();					//destroy the mesh in shapes and free up memory

	return popmodel;
}

