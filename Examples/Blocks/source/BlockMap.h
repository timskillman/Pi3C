#pragma once
#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cimgui.h"
#include "Pi3Cmodel.h"
#include "Pi3Ccolours.h"
#include "Pi3Cshapes.h"
#include <vector>
#include <cctype>
#include "Pi3Cblocks.h"

class BlockMap
{
public:
	BlockMap(Pi3Cresource* resource, int MapSize, int ChunkWidth = 16, int ChunkDepth = 16, int ChunkHeight = 256);
	void createMap(int sx, int bx, int sz, int bz, int trees = 50);
	void createMapMeshes(Pi3Cresource* resource, Pi3Cscene* scene, int sx, int bx, int sz, int bz);
	void updateMapMeshes(Pi3Cresource* resource, Pi3Cscene* scene, int sx, int bx, int sz, int bz);
	void insertBlock(Pi3Cresource* resource, Pi3Cscene* scene, const uint8_t block, const vec3f& newblock);
	int getGroundHeight(const vec3f& pos);

private:

	void createModel(Pi3Cresource* resource, Pi3Cscene* scene, const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId);

	int mapSize = 7;
	int chunkWidth = 16;
	int chunkDepth = 16;
	int chunkHeight = 128;
	int texRef = -1;

	//vec3f chunkCorner;
	Blocks chunkMap;
};

