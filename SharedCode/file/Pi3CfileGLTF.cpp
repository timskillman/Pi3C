#include "Pi3CfileGLTF.h"
#include "Pi3Cutils.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
//#define TINYGLTF_USE_RAPIDJSON

#include "tiny_gltf.h"

Pi3Cgltf::Pi3Cgltf(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename)
{
	bool ret = load(resource, scene, filename);
}

bool Pi3Cgltf::load(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	std::string ext = Pi3Cutils::getExt(filename);
	bool ret = false;

	if (ext == "glb") {
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);
	}
	else if (ext == "gltf") {
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	}

	if (ret) {

	}

	return ret;
}
