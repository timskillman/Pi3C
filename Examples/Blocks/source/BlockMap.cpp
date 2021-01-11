#include "BlockMap.h"

BlockMap::BlockMap(Pi3Cresource* resource, Pi3Cscene* scene, int MapSize, int ChunkWidth, int ChunkDepth, int ChunkHeight)
{
	mapSize = MapSize;
	chunkWidth = ChunkWidth;
	chunkDepth = ChunkDepth;
	chunkHeight = ChunkHeight;
	chunkMap.createBlocks(MapSize, ChunkWidth, ChunkDepth, ChunkHeight);
	//chunkCorner = vec3f(((float)(mapSize * chunkWidth)) / 2, 0, ((float)(mapSize * chunkDepth)) / 2);
	texRef = resource->loadTexture("assets/textures/", "SurvivalCraft256x256.png", false);
}

void BlockMap::createModel(Pi3Cresource* resource, Pi3Cscene* scene, const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId)
{
	Pi3Cmodel model = Pi3Cmodel(resource, mesh, groupId);
	if (model.meshRef >= 0) {
		model.move(pos);
		int32_t modelRef = scene->append3D(model);
	}
}

void BlockMap::createMap(int sx, int bx, int sz, int bz, int trees, uint32_t flags)
{
	//Blocks block(mapSize);
	for (int zb = sz; zb < bz; zb++) {
		for (int xb = sx; xb < bx; xb++) {
			chunkMap.createMapChunk(xb, zb, flags);
		}
	}

	if (trees>0) {
		for (int zb = sz; zb < bz; zb++) {
			for (int xb = sx; xb < bx; xb++) {
				chunkMap.createTrees(xb, zb, trees);
			}
		}
	}
}

void BlockMap::createMapMeshes(Pi3Cresource* resource, Pi3Cscene* scene, int sx, int bx, int sz, int bz)
{
	int blocksId = 1;
	int mph = mapSize / 2;
	for (int zb = sz; zb < bz; zb++) {
		for (int xb = sx; xb < bx; xb++) {
			chunkMap.newMeshChunk(resource, scene, xb, zb, blocksId, texRef);
			blocksId++;
		}
	}
}

void BlockMap::updateMapMeshes(Pi3Cresource* resource, Pi3Cscene* scene, int sx, int bx, int sz, int bz)
{
	int mph = 0; // mapSize / 2;
	for (int zb = sz; zb < bz; zb++) {
		for (int xb = sx; xb < bx; xb++) {
			vec3f moff((float)((xb + mph) * chunkWidth), 0, (float)((zb + mph) * chunkDepth));
			//SDL_Log("moff = %f,%f,%f", moff.x, moff.y, moff.z);
			chunkMap.updateMeshChunk(resource, scene, moff);
		}
	}
}

void BlockMap::insertBlock(Pi3Cresource* resource, Pi3Cscene* scene, const uint8_t block, const vec3f &newblock)
{
	chunkMap.insertBlock(block, newblock);
	chunkMap.updateMeshChunk(resource, scene, newblock);
}

int BlockMap::getGroundHeight(const vec3f& pos)
{
	return chunkMap.getGroundHeight(pos);
}
