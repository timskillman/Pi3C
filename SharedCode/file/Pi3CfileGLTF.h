#pragma once

#include "Pi3Cscene.h"
#include "Pi3Cresource.h"

//using namespace tinygltf;

class Pi3Cgltf {
public:

	Pi3Cgltf() {}
	Pi3Cgltf(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename);

	bool load(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename);
};
