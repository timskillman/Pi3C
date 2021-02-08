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

static Pi3Cmodel ParseMesh(Pi3Cresource* resource, tinygltf::Model& model, const tinygltf::Mesh& mesh) {

	Pi3Cmodel newmodel;
	
	auto& buff = model.buffers[0].data;

	for (size_t i = 0; i < mesh.primitives.size(); i++) {

		const tinygltf::Primitive& primitive = mesh.primitives[i];

		Pi3Cmesh newmesh;

		if (primitive.indices < 0) return newmodel;

		newmesh.mode = primitive.mode;
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

		std::map<std::string, int>::const_iterator it(primitive.attributes.begin());
		std::map<std::string, int>::const_iterator itEnd(primitive.attributes.end());

		int32_t vertOffset = -1;
		int32_t vertCount = -1;
		int32_t normOffset = -1;
		int32_t normCount = -1;
		int32_t colOffset = -1;
		int32_t colCount = -1;
		int32_t uvOffset = -1;
		int32_t uvCount = -1;

		for (; it != itEnd; it++) {
			assert(it->second >= 0);
			const tinygltf::Accessor& accessor = model.accessors[it->second];

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

			int byteOffset = model.bufferViews[accessor.bufferView].byteOffset;

			if (it->first.compare("POSITION") == 0) {
				vertOffset = byteOffset; 
				vertCount = accessor.count;
			}
			if (it->first.compare("NORMAL") == 0) {
				normOffset = byteOffset;
				normCount = accessor.count;
			}
			if (it->first.compare("TEXCOORD_0") == 0) {
				uvOffset = byteOffset;
				uvCount = accessor.count;
			}
			if (it->first.compare("COLOR_0") == 0) {
				colOffset = byteOffset;
				colCount = accessor.count;
			}
		}

		//counts can be different - check here ...

		const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
		int indcount = indexAccessor.count;
		//int indoffset = indexAccessor.byteOffset;
		int byteOffset = model.bufferViews[indexAccessor.bufferView].byteOffset;

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
				index = (unsigned int)*(unsigned short *)&buff[byteOffset + i * 2];
				break;
			case TINYGLTF_COMPONENT_TYPE_INT:
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				index = *(unsigned int *)&buff[byteOffset + i * 4];
				break;
			}

			newmesh.verts[vc++] = *(float*)&buff[vertOffset + index * 12];
			newmesh.verts[vc++] = *(float*)&buff[vertOffset + index * 12 + 4];
			newmesh.verts[vc++] = *(float*)&buff[vertOffset + index * 12 + 8];
			if (normOffset > 0) {
				newmesh.verts[vc++] = *(float*)&buff[normOffset + index * 12];
				newmesh.verts[vc++] = *(float*)&buff[normOffset + index * 12 + 4];
				newmesh.verts[vc++] = *(float*)&buff[normOffset + index * 12 + 8];
			}
			else {
				newmesh.verts[vc++] = 0;
				newmesh.verts[vc++] = 0;
				newmesh.verts[vc++] = 0;
			}
			if (uvOffset > 0) {
				newmesh.verts[vc++] = *(float*)&buff[uvOffset + index * 8];
				newmesh.verts[vc++] = *(float*)&buff[uvOffset + index * 8 + 4];
			}
			else {
				newmesh.verts[vc++] = 0;
				newmesh.verts[vc++] = 0;
			}
			if (colOffset > 0) {
				float fcol = Pi3Cutils::colToFloat((uint8_t)(*(float*)&buff[colOffset + index * 12] * 255.f) |
					((uint8_t)(*(float*)&buff[colOffset + index * 12 + 4] * 255.f) << 8) |
					((uint8_t)(*(float*)&buff[colOffset + index * 12 + 8] * 255.f) << 16) | 0xff000000);
				newmesh.verts[vc++] = fcol;
			}
			else {
				newmesh.verts[vc++] = 0;
			}
		}

		newmodel.appendMeshToGroup(resource, newmesh, 1);
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
