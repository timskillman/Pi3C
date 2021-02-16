#pragma once

#include "Pi3CfileFXGLTF.h"
#include "Pi3Cquaternion.h"
#include "Pi3Cutils.h"

Pi3Cfxgltf::Pi3Cfxgltf(Pi3Cresource* resource, Pi3Cscene* scene, const std::string& filename)
{
	bool ret = load(resource, scene, filename);
}

#define getbuff(buftype, mult, offset)								 \
        *(float*)&buff[buffOffsets[buftype] + index * mult + offset] \

static int typeToSize(fx::gltf::Accessor::Type type)
{
	switch (type) {
	case fx::gltf::Accessor::Type::Scalar: return 1;
	case fx::gltf::Accessor::Type::Vec2: return 2;
	case fx::gltf::Accessor::Type::Vec3: return 3;
	case fx::gltf::Accessor::Type::Vec4: return 4;
	}
	return 0;
}

enum { VERT_OFFSET, NORM_OFFSET, TANG_OFFSET, TEX0_OFFSET, TEX1_OFFSET, COL_OFFSET, JNT_OFFSET, WGT_OFFSET };

static Pi3Cmesh CreateMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const fx::gltf::Primitive& primitive)
{
	int buffOffsets[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	size_t buffElemSizes[8] = { 0,0,0,0,0,0,0,0 };

	Pi3Cmesh newmesh;
	int type = -1;

	for (auto const& attrib : primitive.attributes)
	{
		if (attrib.first == "POSITION") type = VERT_OFFSET;
		else if (attrib.first == "NORMAL") type = NORM_OFFSET;
		else if (attrib.first == "TANGENT") type = TANG_OFFSET;
		else if (attrib.first == "TEXCOORD_0") type = TEX0_OFFSET;
		else if (attrib.first == "TEXCOORD_1") type = TEX1_OFFSET;
		else if (attrib.first == "COLOR_0") type = COL_OFFSET;
		else if (attrib.first == "JOINTS_0") type = JNT_OFFSET;
		else if (attrib.first == "WEIGHTS_0") type = WGT_OFFSET;

		if (type >= 0) {
			const auto& accessor = model.accessors[attrib.second];
			buffOffsets[type] = model.bufferViews[accessor.bufferView].byteOffset + accessor.byteOffset;
			buffElemSizes[type] = typeToSize(accessor.type);
		}

	}

	switch (primitive.mode) {
	case fx::gltf::Primitive::Mode::Points: newmesh.mode = GL_POINTS; break;
	case fx::gltf::Primitive::Mode::Lines: newmesh.mode = GL_LINES; break;
	case fx::gltf::Primitive::Mode::LineLoop: newmesh.mode = GL_LINE_LOOP; break;
	case fx::gltf::Primitive::Mode::LineStrip: newmesh.mode = GL_LINE_STRIP; break;
	case fx::gltf::Primitive::Mode::Triangles: newmesh.mode = GL_TRIANGLES; break;
	case fx::gltf::Primitive::Mode::TriangleStrip: newmesh.mode = GL_TRIANGLE_STRIP; break;
	case fx::gltf::Primitive::Mode::TriangleFan: newmesh.mode = GL_TRIANGLE_FAN; break;
	}

	auto& buff = model.buffers[0].data;

	const auto& indexAccessor = model.accessors[primitive.indices];
	int indcount = indexAccessor.count;
	int byteOffset = model.bufferViews[indexAccessor.bufferView].byteOffset + indexAccessor.byteOffset;

	newmesh.verts.resize(indcount * 9);
	uint32_t& vc = newmesh.vc = 0;
	float white = Pi3Cutils::colToFloat(0xffffffff);

	int itype = 0;
	switch (indexAccessor.componentType) {
	case fx::gltf::Accessor::ComponentType::Byte:
	case fx::gltf::Accessor::ComponentType::UnsignedByte:
		itype = 0;
		break;
	case fx::gltf::Accessor::ComponentType::Short:
	case fx::gltf::Accessor::ComponentType::UnsignedShort:
		itype = 1;
		break;
	case fx::gltf::Accessor::ComponentType::UnsignedInt:
	case fx::gltf::Accessor::ComponentType::Int:
		itype = 2;
		break;
	}

	for (int i = 0; i < indcount; i++) {

		unsigned int index =
			(itype == 0) ? (unsigned int)buff[byteOffset + i] :
			(itype == 1) ? (unsigned int)*(unsigned short*)&buff[byteOffset + i * 2] :
			(unsigned int)*(unsigned int*)&buff[byteOffset + i * 4];

		vec3f point = vec3f(getbuff(VERT_OFFSET, 12, 0), getbuff(VERT_OFFSET, 12, 4), getbuff(VERT_OFFSET, 12, 8));
		vec3f normal = (buffOffsets[NORM_OFFSET] > 0) ? vec3f(getbuff(NORM_OFFSET, 12, 0), getbuff(NORM_OFFSET, 12, 4), getbuff(NORM_OFFSET, 12, 8)) : vec3f();
		vec2f uvs = (buffOffsets[TEX0_OFFSET] > 0) ? vec2f(getbuff(TEX0_OFFSET, 8, 0), getbuff(TEX0_OFFSET, 8, 4)) : vec2f();
		uint32_t col = Pi3Ccolours::White;

		if (buffOffsets[COL_OFFSET] > 0) {
			if (buffElemSizes[COL_OFFSET] == 4) {
				col = (uint8_t)(getbuff(COL_OFFSET, 16, 0) * 255.f) |
					((uint8_t)(getbuff(COL_OFFSET, 16, 4) * 255.f) << 8) |
					((uint8_t)(getbuff(COL_OFFSET, 16, 8) * 255.f) << 16) |
					((uint8_t)(getbuff(COL_OFFSET, 16, 12) * 255.f) << 24);
			}
			else if (buffElemSizes[COL_OFFSET] == 3) {
				col = (uint8_t)(getbuff(COL_OFFSET, 12, 0) * 255.f) |
					((uint8_t)(getbuff(COL_OFFSET, 12, 4) * 255.f) << 8) |
					((uint8_t)(getbuff(COL_OFFSET, 12, 8) * 255.f) << 16) |
					0xff000000;
			}
		}

		Pi3Cutils::storeVNTC(newmesh.verts, vc, point, normal, uvs, col);
		newmesh.bbox.update(point);
	}

	if (buffOffsets[NORM_OFFSET] < 0 && newmesh.mode == GL_TRIANGLES) {
		newmesh.updateNormals(0, newmesh.vc);
	}

	return newmesh;
}

static Pi3Cmodel ParseMesh(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Mesh& mesh, const std::vector<int>& texRefs) {

	Pi3Cmodel newmodel;
	Pi3Cmaterial newmaterial;

	auto& buff = model.buffers[0].data;


	for (size_t i = 0; i < mesh.primitives.size(); i++) {

		const fx::gltf::Primitive& primitive = mesh.primitives[i];

		if (primitive.indices < 0) return newmodel;

		auto* material = (primitive.material >= 0) ? &model.materials[primitive.material] : nullptr;

		if (material) {
			newmaterial = *resource->defaultMaterial();
			newmaterial.name = material->name;
			newmaterial.groupID = 1;

			const auto& basecol = material->pbrMetallicRoughness.baseColorFactor;
			if (basecol.size() == 4) {
				newmaterial.SetColDiffuse(vec4f((float)basecol[0], (float)basecol[1], (float)basecol[2], (float)basecol[3]));
			}

			//const auto& baseshine = material.pbrMetallicRoughness.metallicFactor;

			int tref = material->pbrMetallicRoughness.baseColorTexture.index;
			if (tref >= 0 && tref < texRefs.size()) {
				newmaterial.texRef = texRefs[tref];
				newmaterial.texID = resource->getTextureID(newmaterial.texRef);
				std::shared_ptr<Pi3Ctexture> Texture = resource->textures[newmaterial.texRef];
				newmaterial.texWidth = Texture->GetWidth();
				newmaterial.texHeight = Texture->GetHeight();
				newmaterial.alpha = 0.999f;
			}
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

static Pi3Cmodel ParseNode(Pi3Cresource* resource, fx::gltf::Document& model, fx::gltf::Node& node, const std::vector<int>& texRefs) {
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
	//else {
		// Assume Trans x Rotate x Scale order
		if (node.scale.size() == 3) {
			matrix.scale(vec3f(node.scale[0], node.scale[1], node.scale[2]));
		}

		if (node.rotation.size() == 4) {
			Pi3Cmatrix mtx;
			Pi3Cquaternion quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
			matrix = matrix * quat.toMatrix();
		}

		if (node.translation.size() == 3) {
			matrix.Translate(vec3f(node.translation[0], node.translation[1], node.translation[2]));
		}
	//}

	if (node.mesh > -1) {
		assert(node.mesh < model.meshes.size());
		newmodel = ParseMesh(resource, model, model.meshes[node.mesh], texRefs);
	}

	for (size_t i = 0; i < node.children.size(); i++) {
		assert(node.children[i] < model.nodes.size());
		Pi3Cmodel nodemod = ParseNode(resource, model, model.nodes[node.children[i]], texRefs);
		newmodel.append(resource, nodemod);
	}

	newmodel.matrix = matrix;
	return newmodel;
}

bool Pi3Cfxgltf::load(Pi3Cresource* resource, Pi3Cscene* piscene, const std::string& filepath)
{
	std::string err;
	std::string warn;

	std::string ext = Pi3Cutils::getExt(filepath);
	std::string name = Pi3Cutils::extractName(filepath);

	bool ret = true;

	if (ext == "glb") {
		model = fx::gltf::LoadFromBinary(filepath);
	}
	else if (ext == "gltf") {
		model = fx::gltf::LoadFromText(filepath);
	}

	if (ret) {
		//Get textures ...
		std::vector<int> texRefs(0);
		if (model.images.size() > 0) {
			for (size_t i = 0; i < model.images.size(); i++) {
				std::string uri = model.images[i].uri;
				if (uri != "") {
					std::string texname = filepath + "-" + uri;
					int32_t ft = resource->findTextureByName(texname);
					if (ft < 0) {
						std::string path = Pi3Cutils::extractPath(filepath);

						std::shared_ptr<Pi3Ctexture> texture;
						texture.reset(new Pi3Ctexture());
						texture->loadFromFile(Pi3Cutils::filepath(path, uri).c_str());

						int iw = texture->GetWidth(), ih = texture->GetHeight(), sc = 1;
						//while (iw > 2048 || ih > 2048) { iw /= 2; ih /= 2; sc *= 2; }

						uint32_t* tpix = (uint32_t*)texture->GetTexels();
						//if (sc > 1) {
						//	int q = 0, sp = 0;
						//	for (int sy = 0; sy < image.height; sy += sc) {
						//		for (int sx = 0; sx < image.width; sx += sc) {
						//			int q = sx + sy * image.width;
						//			tpix[sp++] = srcimg[q];
						//		}
						//	}
						//}
						//else
						//{
						//	memcpy(tpix, srcimg, iw * ih * 4);
						//}

						Pi3Cutils::flipImage((uint8_t*)tpix, iw, ih);
						texture->name = texname;
						ft = resource->addTexture(texture);
						Pi3Cmaterial mat = *resource->defaultMaterial();
						mat.texRef = ft;
						mat.texName = texname;
						mat.texWidth = iw;
						mat.texHeight = ih;
						mat.name = texname;
						mat.alpha = 0.99f;
						mat.illum = 2;
						resource->addMaterial(mat);
					}
					texRefs.push_back(ft);
				}
			}
		}

		Pi3Cmodel rootmodel;
		//int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
		auto& scene = model.scenes[0];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			rootmodel.append(resource, ParseNode(resource, model, model.nodes[scene.nodes[i]], texRefs));
		}
		piscene->append3D(rootmodel);

	}
	return ret;
}
