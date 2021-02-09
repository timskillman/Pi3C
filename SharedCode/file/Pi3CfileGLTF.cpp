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

//int mode = -1;
//if (primitive.mode == TINYGLTF_MODE_TRIANGLES) {
//	mymesh.mode = GL_TRIANGLES;
//}
//else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_STRIP) {
//	mymesh.mode = GL_TRIANGLE_STRIP;
//}
//else if (primitive.mode == TINYGLTF_MODE_TRIANGLE_FAN) {
//	mymesh.mode = GL_TRIANGLE_FAN;
//}
//else if (primitive.mode == TINYGLTF_MODE_POINTS) {
//	mymesh.mode = GL_POINTS;
//}
//else if (primitive.mode == TINYGLTF_MODE_LINE) {
//	mymesh.mode = GL_LINES;
//}
//else if (primitive.mode == TINYGLTF_MODE_LINE_LOOP) {
//	mymesh.mode = GL_LINE_LOOP;
//}
//else {
//	assert(0);
//}

//int size = 1;
//switch (accessor.type) {
//case TINYGLTF_TYPE_SCALAR: size = 1; break;
//case TINYGLTF_TYPE_VEC2: size = 2; break;
//case TINYGLTF_TYPE_VEC3: size = 3; break;
//case TINYGLTF_TYPE_VEC4: size = 4; break;
//default: assert(0);
//}

//SDL_Log("Prim Accessor:%s, accessor:%d, bufferView:%d, byteOffset:%d, count:%d, size:%d",  
//	it->first.c_str(), it->second, accessor.bufferView, accessor.byteOffset, accessor.count, size);

static size_t ComponentTypeByteSize(int type) {
	switch (type) {
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	case TINYGLTF_COMPONENT_TYPE_BYTE:
		return sizeof(char);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
	case TINYGLTF_COMPONENT_TYPE_SHORT:
		return sizeof(short);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
	case TINYGLTF_COMPONENT_TYPE_INT:
		return sizeof(int);
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
		return sizeof(float);
	case TINYGLTF_COMPONENT_TYPE_DOUBLE:
		return sizeof(double);
	default:
		return 0;
	}
}

enum { VERT_OFFSET, NORM_OFFSET, TANG_OFFSET, TEX0_OFFSET, TEX1_OFFSET, COL_OFFSET, JNT_OFFSET, WGT_OFFSET };

static Pi3Cmesh CreateMesh(tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	int buffOffsets[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	size_t buffElemSizes[8] = { 0,0,0,0,0,0,0,0 };

	std::map<std::string, int>::const_iterator it(primitive.attributes.begin());
	std::map<std::string, int>::const_iterator itEnd(primitive.attributes.end());

	for (; it != itEnd; it++) {
		assert(it->second >= 0);

		int type = -1;
		if (it->first.compare("POSITION") == 0) type = VERT_OFFSET;
		if (it->first.compare("NORMAL") == 0) type = NORM_OFFSET;
		if (it->first.compare("TANGENT") == 0) type = TANG_OFFSET;
		if (it->first.compare("TEXCOORD_0") == 0) type = TEX0_OFFSET;
		if (it->first.compare("TEXCOORD_1") == 0) type = TEX1_OFFSET;
		if (it->first.compare("COLOR_0") == 0) type = COL_OFFSET;
		if (it->first.compare("JOINTS_0") == 0) type = JNT_OFFSET;
		if (it->first.compare("WEIGHTS_0") == 0) type = WGT_OFFSET;

		if (type >= 0) {
			const tinygltf::Accessor& accessor = model.accessors[it->second];
			buffOffsets[type] = model.bufferViews[accessor.bufferView].byteOffset + accessor.byteOffset;
			buffElemSizes[type] = ComponentTypeByteSize(accessor.componentType);
		}
		else
		{
			//throw ()
		}
	}

	Pi3Cmesh newmesh;
	newmesh.mode = primitive.mode;

	auto& buff = model.buffers[0].data;

	const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
	int indcount = indexAccessor.count;
	int byteOffset = model.bufferViews[indexAccessor.bufferView].byteOffset +indexAccessor.byteOffset;

	newmesh.verts.resize(indcount * 9);
	uint32_t& vc = newmesh.vc = 0;

	for (int i = 0; i < indcount; i++) {
		unsigned int index = 0;
		switch (indexAccessor.componentType) {
		case TINYGLTF_COMPONENT_TYPE_BYTE:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			index = (unsigned int)buff[byteOffset + i];
			break;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			index = (unsigned int)*(unsigned short*)&buff[byteOffset + i * 2];
			break;
		case TINYGLTF_COMPONENT_TYPE_INT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			index = (unsigned int)*(unsigned int*)&buff[byteOffset + i * 4];
			break;
		}

		vec3f point = vec3f(*(float*)&buff[buffOffsets[VERT_OFFSET] + index * 12], *(float*)&buff[buffOffsets[VERT_OFFSET] + index * 12 + 4], *(float*)&buff[buffOffsets[VERT_OFFSET] + index * 12 + 8]);
		newmesh.verts[vc++] = point.x; // *(float*)&buff[vertOffset + index * 12];
		newmesh.verts[vc++] = point.y; // *(float*)&buff[vertOffset + index * 12 + 4];
		newmesh.verts[vc++] = point.z; // *(float*)&buff[vertOffset + index * 12 + 8];
		newmesh.bbox.update(point);

		if (buffOffsets[NORM_OFFSET] > 0) {
			newmesh.verts[vc++] = *(float*)&buff[buffOffsets[NORM_OFFSET] + index * 12];
			newmesh.verts[vc++] = *(float*)&buff[buffOffsets[NORM_OFFSET] + index * 12 + 4];
			newmesh.verts[vc++] = *(float*)&buff[buffOffsets[NORM_OFFSET] + index * 12 + 8];
		}
		else {
			newmesh.verts[vc++] = 0;
			newmesh.verts[vc++] = 0;
			newmesh.verts[vc++] = 0;
		}
		if (buffOffsets[TEX0_OFFSET] > 0) {
			newmesh.verts[vc++] = *(float*)&buff[buffOffsets[TEX0_OFFSET] + index * 8];
			newmesh.verts[vc++] = *(float*)&buff[buffOffsets[TEX0_OFFSET] + index * 8 + 4];
		}
		else {
			newmesh.verts[vc++] = 0;
			newmesh.verts[vc++] = 0;
		}

		if (buffOffsets[COL_OFFSET] > 0) {
			uint32_t col = 0;
			//if (buffElemSizes[COL_OFFSET] == 4) {
			//	col = (uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 16] * 255.f) |
			//		((uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 16 + 4] * 255.f) << 8) |
			//		((uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 16 + 8] * 255.f) << 16) |
			//		((uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 16 + 12] * 255.f) << 24);
			//}
			//else if (buffElemSizes[COL_OFFSET] == 3) {
				col = (uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 12] * 255.f) |
					((uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 12 + 4] * 255.f) << 8) |
					((uint8_t)(*(float*)&buff[buffOffsets[COL_OFFSET] + index * 12 + 8] * 255.f) << 16) |
					0xff000000;
			//}
			newmesh.verts[vc++] = Pi3Cutils::colToFloat(col);
		}
		else {
			newmesh.verts[vc++] = 0;
		}
	}

	if (buffOffsets[NORM_OFFSET] < 0 && newmesh.mode == GL_TRIANGLES) {
		newmesh.updateNormals(0, newmesh.vc);
	}

	return newmesh;
}

static Pi3Cmodel ParseMesh(Pi3Cresource* resource, tinygltf::Model& model, const tinygltf::Mesh& mesh) {

	Pi3Cmodel newmodel;
	
	auto& buff = model.buffers[0].data;


	for (size_t i = 0; i < mesh.primitives.size(); i++) {

		const tinygltf::Primitive& primitive = mesh.primitives[i];

		if (primitive.indices < 0) return newmodel;

		Pi3Cmesh newmesh = CreateMesh(model, primitive);
		newmodel.appendMeshToGroup(resource, newmesh, 1);

		//Check if targets exist, add target meshes to mesh group

		int targets = primitive.targets.size();

		if (targets > 0) {

			auto prim = primitive;
			newmodel.animated = true;

			for (int targRef = 0; targRef < targets; targRef++) {

				auto& targattr = primitive.targets[targRef];

				std::map<std::string, int>::const_iterator it(targattr.begin());
				std::map<std::string, int>::const_iterator itEnd(targattr.end());
				for (; it != itEnd; it++) {
					std::map<std::string, int>::iterator itf = prim.attributes.find(it->first);
					if (itf != prim.attributes.end()) itf->second = it->second;
				}

				Pi3Cmesh targMesh = CreateMesh(model, prim);
				for (size_t v=0; v < targMesh.vc; v+=targMesh.stride) {
					targMesh.verts[v] += newmesh.verts[v];
					targMesh.verts[v + 1] += newmesh.verts[v + 1];
					targMesh.verts[v + 2] += newmesh.verts[v + 2];
				}
				targMesh.updateNormals(0, targMesh.vc);
				newmodel.appendMeshToGroup(resource, targMesh, 1);
			}
		}
	}

	return newmodel;
}


static Pi3Cmodel ParseNode(Pi3Cresource* resource, tinygltf::Model& model, const tinygltf::Node& node) {
	// Apply xform

	Pi3Cmodel newmodel;

	const auto* nmat = node.matrix.data();
	Pi3Cmatrix matrix;

	if (node.matrix.size() == 16) {
		matrix.set(nmat[0], nmat[1], nmat[2], nmat[3],
			nmat[4], nmat[5], nmat[6], nmat[7],
			nmat[8], nmat[9], nmat[10], nmat[11],
			nmat[12], nmat[13], nmat[14], nmat[15]);
	}
	else {
		// Assume Trans x Rotate x Scale order
		if (node.scale.size() == 3) {
			matrix.scale(vec3f(node.scale[0], node.scale[1], node.scale[2]));
		}

		if (node.rotation.size() == 4) {
			matrix.rotateAXYZ(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
		}

		if (node.translation.size() == 3) {
			matrix.Translate(vec3f(node.translation[0], node.translation[1], node.translation[2]));
		}
	}

	if (node.mesh > -1) {
		assert(node.mesh < model.meshes.size());
		newmodel = ParseMesh(resource, model, model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert(node.children[i] < model.nodes.size());
		Pi3Cmodel nodemod = ParseNode(resource, model, model.nodes[node.children[i]]);
		newmodel.append(resource, nodemod);
	}

	newmodel.matrix = matrix;
	return newmodel;
}

bool Pi3Cgltf::load(Pi3Cresource* resource, Pi3Cscene* piscene, const std::string& filename)
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
		assert(model.scenes.size() > 0);

		Pi3Cmodel rootmodel;
		Pi3Cmatrix matrix;
		int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
		const tinygltf::Scene& scene = model.scenes[scene_to_display];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			rootmodel.append(resource, ParseNode(resource, model, model.nodes[scene.nodes[i]]));
		}
		piscene->append3D(rootmodel);
	}

	return ret;
}
