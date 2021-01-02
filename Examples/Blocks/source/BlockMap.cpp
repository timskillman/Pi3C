#include "BlockMap.h"

void BlockMap::createModel(Pi3Cresource* resource, Pi3Cscene* scene, const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId)
{
	Pi3Cmodel model = Pi3Cmodel(resource, mesh, groupId);
	if (model.meshRef >= 0) {
		model.move(pos);
		int32_t modelRef = scene->append3D(model);
	}
}

void BlockMap::createMap(Pi3Cresource* resource, Pi3Cscene* scene, Blocks* block, int mapSize, const vec3f& pos)
{
	//Blocks block(mapSize);
	for (int zb = 0; zb < mapSize; zb++) {
		for (int xb = 0; xb < mapSize; xb++) {
			block->createMapChunk(xb, zb);
		}
	}

	for (int zb = 0; zb < mapSize; zb++) {
		for (int xb = 0; xb < mapSize; xb++) {
			block->createTrees(xb, zb, 30);
		}
	}

	int blocksId = 1;
	int texRef = resource->loadTexture("assets/textures/", "SurvivalCraft256x256.png", false);

	for (int zb = 1; zb < mapSize - 1; zb++) {
		for (int xb = 1; xb < mapSize - 1; xb++) {
			Pi3Cmesh blockmesh("ChunkMesh");
			block->createMeshChunk(blockmesh, xb, zb);
			createModel(resource, scene, blockmesh, pos, blocksId);
			Pi3Cmodel& model = scene->models.back();
			model.name = "ChunkModel";
			model.material.texRef = texRef;
			model.material.texID = resource->getTextureID(texRef);
			//blocksId++;
		}
	}
}