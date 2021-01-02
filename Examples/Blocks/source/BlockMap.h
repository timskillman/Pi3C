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
	BlockMap() {}

	void createModel(Pi3Cresource* resource, Pi3Cscene* scene, const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId);
	void createMap(Pi3Cresource* resource, Pi3Cscene* scene, Blocks* block, int mapSize, const vec3f& pos);

private:

};

