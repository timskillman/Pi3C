#pragma once

#include "Pi3CfileFXGLTF.h"
//#include "Pi3Cquaternion.h"
#include "Pi3Cutils.h"

Pi3Cfxgltf::Pi3Cfxgltf(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename, const std::function<void(float)> showProgressCB)
{
	bool ret = load(resource, scene, filename, showProgressCB);
}

#define getfloat(buftype, mult, offset)								    \
        *(float*)&buffers[buffOffsets[buftype] + index * mult + offset] \

int Pi3Cfxgltf::typeToSize(fx::gltf::Accessor::Type type)
{
	switch (type) {
	case fx::gltf::Accessor::Type::Scalar: return 1;
	case fx::gltf::Accessor::Type::Vec2: return 2;
	case fx::gltf::Accessor::Type::Vec3: return 3;
	case fx::gltf::Accessor::Type::Vec4: return 4;
	}
	return 0;
}

uint16_t Pi3Cfxgltf::GetGLmodeFromPrimitiveMode(const fx::gltf::Primitive::Mode& primmode)
{
	switch (primmode) {
	case fx::gltf::Primitive::Mode::Points: return GL_POINTS;
	case fx::gltf::Primitive::Mode::Lines: return GL_LINES;
	case fx::gltf::Primitive::Mode::LineLoop: return GL_LINE_LOOP;
	case fx::gltf::Primitive::Mode::LineStrip: return GL_LINE_STRIP;
	case fx::gltf::Primitive::Mode::Triangles: return GL_TRIANGLES;
	case fx::gltf::Primitive::Mode::TriangleStrip: return GL_TRIANGLE_STRIP;
	case fx::gltf::Primitive::Mode::TriangleFan: return GL_TRIANGLE_FAN;
	}
}

int Pi3Cfxgltf::GetAttributeOffset(const std::string& attribstr)
{
	int type = -1;
	if (attribstr == "POSITION") type = VERT_OFFSET;
	else if (attribstr == "NORMAL") type = NORM_OFFSET;
	else if (attribstr == "TANGENT") type = TANG_OFFSET;
	else if (attribstr == "TEXCOORD_0") type = TEX0_OFFSET;
	else if (attribstr == "TEXCOORD_1") type = TEX1_OFFSET;
	else if (attribstr == "COLOR_0") type = COL_OFFSET;
	else if (attribstr == "JOINTS_0") type = JNT_OFFSET;
	else if (attribstr == "WEIGHTS_0") type = WGT_OFFSET;
	return type;
}

int Pi3Cfxgltf::GetComponentSize(const fx::gltf::Accessor::ComponentType componentType)
{
	switch (componentType) {
	case fx::gltf::Accessor::ComponentType::Byte:
	case fx::gltf::Accessor::ComponentType::UnsignedByte:
		return 1;
	case fx::gltf::Accessor::ComponentType::Short:
	case fx::gltf::Accessor::ComponentType::UnsignedShort:
		return 2;
	case fx::gltf::Accessor::ComponentType::UnsignedInt:
	case fx::gltf::Accessor::ComponentType::Int:
	case fx::gltf::Accessor::ComponentType::Float:
		return 4;
	}
	return 0;
}

unsigned int Pi3Cfxgltf::GetIndexByType(const int itype, std::vector<uint8_t>& buffers, int byteOffset, int index)
{
	unsigned int indexOut = 0;
	memcpy(&indexOut, &buffers[byteOffset + index * itype], itype);
	return indexOut;
}

uint32_t Pi3Cfxgltf::GetColour(const std::vector<int>& buffOffsets, const std::vector<int>& buffElemSizes, unsigned int index, std::vector<uint8_t>& buffers)
{
	//TODO - several other variants
	uint32_t col = Pi3Ccolours::White;
	if (buffElemSizes[COL_OFFSET] == 4) {
		col = (uint8_t)(getfloat(COL_OFFSET, 16, 0) * 255.f) |
			((uint8_t)(getfloat(COL_OFFSET, 16, 4) * 255.f) << 8) |
			((uint8_t)(getfloat(COL_OFFSET, 16, 8) * 255.f) << 16) |
			((uint8_t)(getfloat(COL_OFFSET, 16, 12) * 255.f) << 24);
	}
	else if (buffElemSizes[COL_OFFSET] == 3) {
		col = (uint8_t)(getfloat(COL_OFFSET, 12, 0) * 255.f) |
			((uint8_t)(getfloat(COL_OFFSET, 12, 4) * 255.f) << 8) |
			((uint8_t)(getfloat(COL_OFFSET, 12, 8) * 255.f) << 16) |
			0xff000000;
	}
	return col;
}

void Pi3Cfxgltf::SetAttributeOffsets(const fx::gltf::Primitive& primitive, std::vector<int>& buffOffsets, std::vector<int>& buffElemSizes)
{
	for (auto const& attrib : primitive.attributes)
	{
		int type = GetAttributeOffset(attrib.first);
		if (type >= 0) {
			const auto& accessor = model.accessors[attrib.second];
			buffOffsets[type] = model.bufferViews[accessor.bufferView].byteOffset + accessor.byteOffset;
			buffElemSizes[type] = typeToSize(accessor.type);
		}
	}
}

Pi3Cmesh Pi3Cfxgltf::CreateMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const fx::gltf::Primitive& primitive)
{
	auto& buffers = model.buffers[0].data;

	std::vector<int> buffOffsets(8, -1);
	std::vector<int> buffElemSizes(8, 0);
	SetAttributeOffsets(primitive, buffOffsets, buffElemSizes);

	const auto& indexAccessor = model.accessors[primitive.indices];
	int byteOffset = model.bufferViews[indexAccessor.bufferView].byteOffset + indexAccessor.byteOffset;
	int indcount = indexAccessor.count;

	Pi3Cmesh newmesh;
	newmesh.stride = 9;
	newmesh.mode = GetGLmodeFromPrimitiveMode(primitive.mode);
	newmesh.verts.resize(indcount * newmesh.stride);
	newmesh.vc = 0;

	int indexSize = GetComponentSize(indexAccessor.componentType);

	for (int i = 0; i < indcount; i++) {

		unsigned int index = GetIndexByType(indexSize, buffers, byteOffset, i);

		vec3f position = vec3f(getfloat(VERT_OFFSET, 12, 0), getfloat(VERT_OFFSET, 12, 4), getfloat(VERT_OFFSET, 12, 8));
		vec3f normal = (buffOffsets[NORM_OFFSET] > 0) ? vec3f(getfloat(NORM_OFFSET, 12, 0), getfloat(NORM_OFFSET, 12, 4), getfloat(NORM_OFFSET, 12, 8)) : vec3f();
		vec2f texcoord = (buffOffsets[TEX0_OFFSET] > 0) ? vec2f(getfloat(TEX0_OFFSET, 8, 0), getfloat(TEX0_OFFSET, 8, 4)) : vec2f();
		uint32_t colour = (buffOffsets[COL_OFFSET] > 0) ? GetColour(buffOffsets, buffElemSizes, index, buffers) : Pi3Ccolours::White;

		Pi3Cutils::storeVNTC(newmesh.verts, newmesh.vc, position, normal, texcoord, colour);
		newmesh.bbox.update(position);
	}

	if (buffOffsets[NORM_OFFSET] < 0 && newmesh.mode == GL_TRIANGLES) {
		newmesh.updateNormals(0, newmesh.vc);
	}

	return newmesh;
}

Pi3Cmodel Pi3Cfxgltf::ParseMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const std::vector<int>& texRefs) 
{

	Pi3Cmodel newmodel;
	Pi3Cmaterial newmaterial;

	auto& buff = model.buffers[0].data;

	for (size_t i = 0; i < mesh.primitives.size(); i++) {

		const fx::gltf::Primitive& primitive = mesh.primitives[i];

		if (primitive.indices < 0) return newmodel;

		auto* material = (primitive.material >= 0) ? &model.materials[primitive.material] : nullptr;

		if (material) {
			int tref = material->pbrMetallicRoughness.baseColorTexture.index;
			if (tref >= 0 && tref < texRefs.size()) {
				newmaterial = resource->createMaterialFromTexture(texRefs[tref]);
			}
			else {
				newmaterial = *resource->defaultMaterial();
			}
			newmaterial.name = material->name;
			newmaterial.groupID = 1;

			const auto& basecol = material->pbrMetallicRoughness.baseColorFactor;
			if (basecol.size() == 4) {
				newmaterial.SetColDiffuse(vec4f((float)basecol[0], (float)basecol[1], (float)basecol[2], (float)basecol[3]));
			}

			//const auto& baseshine = material.pbrMetallicRoughness.metallicFactor;
		}

		Pi3Cmesh basemesh = CreateMesh(resource, model, mesh, primitive);

		int gref = newmodel.appendMeshToGroup(resource, basemesh, 1);
		if (material) {
			Pi3Cmodel& gpmodel = newmodel.group[gref];
			gpmodel.material = newmaterial;
		}

		//Check if targets exist, add target meshes to mesh group

		int targets = primitive.targets.size();

		if (targets > 0) {

			auto prim = primitive;
			newmodel.animgroup = true;

			for (int targRef = 0; targRef < targets; targRef++) {

				const fx::gltf::Attributes& targattr = primitive.targets[targRef];

				std::unordered_map<std::string, uint32_t>::const_iterator it(targattr.begin());
				std::unordered_map<std::string, uint32_t>::const_iterator itEnd(targattr.end());
				for (; it != itEnd; it++) {
					std::unordered_map<std::string, uint32_t>::iterator itf = prim.attributes.find(it->first);
					if (itf != prim.attributes.end()) itf->second = it->second;
				}

				Pi3Cmesh targMesh = CreateMesh(resource, model, mesh, prim);
				//Add targMesh verts to base mesh verts
				for (size_t v = 0; v < targMesh.vc; v += targMesh.stride) {
					targMesh.verts[v] += basemesh.verts[v];
					targMesh.verts[v + 1] += basemesh.verts[v + 1];
					targMesh.verts[v + 2] += basemesh.verts[v + 2];
				}
				targMesh.updateNormals(0, targMesh.vc);
				int gref = newmodel.appendMeshToGroup(resource, targMesh, 1);
				if (material) {
					Pi3Cmodel& gpmodel = newmodel.group[gref];
					gpmodel.material = newmaterial;
				}
			}
		}
	}

	return newmodel;
}

Pi3Cmodel Pi3Cfxgltf::ParseNode(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Node& node, const std::vector<int>& texRefs)
{
	// Apply xform

	Pi3Cmodel newmodel;
	Pi3Cmatrix matrix;

	const auto* nmat = node.matrix.data();
	if (node.matrix.size() == 16) {
		matrix.set(nmat[0], nmat[1], nmat[2], nmat[3],
			nmat[4], nmat[5], nmat[6], nmat[7],
			nmat[8], nmat[9], nmat[10], nmat[11],
			nmat[12], nmat[13], nmat[14], nmat[15]);
	}

	// Assume Trans x Rotate x Scale order
	if (node.scale.size() == 3) {
		matrix.scale(vec3f(node.scale[0], node.scale[1], node.scale[2]));
	}

	if (node.rotation.size() == 4) {
		Pi3Cmatrix qmtx;
		qmtx.fromQuaternion(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
		//Pi3Cquaternion quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
		matrix = matrix * qmtx; // quat.toMatrix();
	}

	if (node.translation.size() == 3) {
		matrix.Translate(vec3f(node.translation[0], node.translation[1], node.translation[2]));
	}

	if (node.mesh > -1) {
		assert(node.mesh < model.meshes.size());
		newmodel = ParseMesh(resource, model, model.meshes[node.mesh], texRefs);
	}

	float pstep = 40.f / (float) node.children.size();

	for (size_t i = 0; i < node.children.size(); i++) {

		if (showProgressCB != nullptr) showProgressCB(60.f + i*pstep);

		assert(node.children[i] < model.nodes.size());
		Pi3Cmodel nodemod = ParseNode(resource, model, model.nodes[node.children[i]], texRefs);
		newmodel.append(resource, nodemod);
	}

	newmodel.matrix = matrix;
	return newmodel;
}

void Pi3Cfxgltf::loadTextures(Pi3Cresource* resource, const std::string& filepath)
{
	if (model.images.size() > 0) {

		float pstep = 20.f / (float)model.images.size();

		for (size_t i = 0; i < model.images.size(); i++) {

			if (showProgressCB != nullptr) showProgressCB(40.f + i * pstep);

			std::string uri = model.images[i].uri;
			int buffViewRef = model.images[i].bufferView;
			std::string texname = "";
			int32_t texref = -1;
			Pi3Ctexture texture;

			if (uri != "" || buffViewRef >= 0) {
				if (uri == "") {
					texname = filepath + "-" + model.images[i].name;
					texref = resource->findTextureByName(texname);
					if (texref < 0) {
						auto& buff = model.buffers[0].data;
						int byteOffset = model.bufferViews[buffViewRef].byteOffset;// +indexAccessor.byteOffset;
						texture.loadFromMemory(&buff[byteOffset], model.bufferViews[buffViewRef].byteLength);
					}
				}
				else
				{
					texname = filepath + "-" + uri;
					texref = resource->findTextureByName(texname);
					if (texref < 0) {
						std::string path = Pi3Cutils::extractPath(filepath);
						texture.loadFromFile(Pi3Cutils::filepath(path, uri).c_str());
					}
				}

				//ensure texture is less than 2048x2048 (max for Pi Zero)
				int iw = texture.GetWidth(), ih = texture.GetHeight(), sc = 0;
				while (iw > 2048 || ih > 2048) { iw /= 2; ih /= 2; sc++; }
				if (sc > 0) texture.shrinkPow2(sc);

				texture.flip();

				texture.name = texname;
				texref = resource->addTexture(texture);
				Pi3Cmaterial mat = resource->createMaterialFromTexture(texref);
				resource->addMaterial(mat);

				texRefs.push_back(texref);
			}
		}
	}
}

bool Pi3Cfxgltf::load(Pi3Cresource* resource, Pi3Cscene* piscene, const std::string& filepath, const std::function<void(float)> showProgress)
{
	showProgressCB = showProgress;

	std::string ext = Pi3Cutils::getExt(filepath);

	if (ext == "glb") {
		model = fx::gltf::LoadFromBinary(filepath);
	}
	else if (ext == "gltf") {
		model = fx::gltf::LoadFromText(filepath);
	}

	if (showProgressCB != nullptr) showProgressCB(40.f);

	loadTextures(resource, filepath);

	if (showProgressCB != nullptr) showProgressCB(60.f);

	Pi3Cmodel rootmodel;
	//int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
	auto& scene = model.scenes[0];
	for (size_t i = 0; i < scene.nodes.size(); i++) {
		rootmodel.append(resource, ParseNode(resource, model, model.nodes[scene.nodes[i]], texRefs));
	}
	piscene->append3D(rootmodel);

	return true;
}
