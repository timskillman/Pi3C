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
	Pi3Cfxgltf(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename, const std::function<void(float)> showProgressCB = nullptr);

	bool load(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename, const std::function<void(float)> showProgressCB = nullptr);

private:

	fx::gltf::Document model{};
	std::vector<int> texRefs;

	std::function<void(float)> showProgressCB = nullptr;

	Pi3Cmodel ParseNode(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Node& node, const std::vector<int>& texRefs);
	Pi3Cmodel ParseMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const std::vector<int>& texRefs);
	Pi3Cmesh CreateMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const fx::gltf::Primitive& primitive);

	int typeToSize(fx::gltf::Accessor::Type type);
	uint16_t GetGLmodeFromPrimitiveMode(const fx::gltf::Primitive::Mode& primmode);
	int GetAttributeOffset(const std::string& attribstr);
	int GetComponentSize(const fx::gltf::Accessor::ComponentType componentType);
	unsigned int GetIndexByType(const int itype, std::vector<uint8_t>& buffers, int byteOffset, int index);
	uint32_t GetColour(const std::vector<int>& buffOffsets, const std::vector<int>& buffElemSizes, unsigned int index, std::vector<uint8_t>& buffers);
	void SetAttributeOffsets(const fx::gltf::Primitive& primitive, std::vector<int>& buffOffsets, std::vector<int>& buffElemSizes);
	void loadTextures(Pi3Cresource* resource, const std::string& filepath);

	//template <typename T>
	//int32_t read(T& uint8_t* buffer) {
	//	stream->read(reinterpret_cast<char*>(&value), sizeof(T));
	//}

	enum { VERT_OFFSET, NORM_OFFSET, TANG_OFFSET, TEX0_OFFSET, TEX1_OFFSET, COL_OFFSET, JNT_OFFSET, WGT_OFFSET };
};
