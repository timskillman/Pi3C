#pragma once

#include "Pi3Cscene.h"
#include "Pi3Cresource.h"
#include <fx/gltf.h>
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Pi3Ccolours.h"

using namespace fx::gltf;

class Pi3Cfxgltf {
public:

	Pi3Cfxgltf() {}
	Pi3Cfxgltf(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename);

	bool load(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename);

	fx::gltf::Document model{};
};
