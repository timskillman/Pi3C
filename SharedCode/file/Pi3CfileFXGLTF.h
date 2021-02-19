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

private:

	fx::gltf::Document model{};

	Pi3Cmodel ParseNode(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Node& node, const std::vector<int>& texRefs);
	Pi3Cmodel ParseMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const std::vector<int>& texRefs);
	Pi3Cmesh CreateMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const fx::gltf::Primitive& primitive);

	int typeToSize(fx::gltf::Accessor::Type type);
	uint16_t GetGLmodeFromPrimitiveMode(const fx::gltf::Primitive::Mode& primmode);
	int GetAttributeOffset(const std::string& attribstr);
	int GetComponentType(const fx::gltf::Accessor::ComponentType componentType);
	unsigned int GetIndexByType(const int itype, std::vector<uint8_t>& buffers, int byteOffset, int index);
	uint32_t GetColour(const std::vector<int>& buffOffsets, const std::vector<int>& buffElemSizes, unsigned int index, std::vector<uint8_t>& buffers);
	void SetAttributeOffsets(const fx::gltf::Primitive& primitive, std::vector<int>& buffOffsets, std::vector<int>& buffElemSizes);

	enum { VERT_OFFSET, NORM_OFFSET, TANG_OFFSET, TEX0_OFFSET, TEX1_OFFSET, COL_OFFSET, JNT_OFFSET, WGT_OFFSET };
};
