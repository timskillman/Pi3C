#include "Pi3CfileGLTF.h"
#include "Pi3Cutils.h"
#include "Pi3Cquaternion.h"

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

static int typeToSize(int type)
{
	switch (type) {
	case TINYGLTF_TYPE_SCALAR: return 1;
	case TINYGLTF_TYPE_VEC2: return 2;
	case TINYGLTF_TYPE_VEC3: return 3;
	case TINYGLTF_TYPE_VEC4: return 4;
	}
	return 0;
}

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

#define getbuff(buftype, mult, offset)								 \
        *(float*)&buff[buffOffsets[buftype] + index * mult + offset] \


enum { VERT_OFFSET, NORM_OFFSET, TANG_OFFSET, TEX0_OFFSET, TEX1_OFFSET, COL_OFFSET, JNT_OFFSET, WGT_OFFSET };


static Pi3Cmesh CreateMesh(Pi3Cresource* resource, tinygltf::Model& model, const tinygltf::Primitive& primitive)
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
			buffElemSizes[type] = typeToSize(accessor.type);
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
	float white = Pi3Cutils::colToFloat(0xffffffff);

	int itype = 0;
	switch (indexAccessor.componentType) {
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		itype = 0;
		break;
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		itype = 1;
		break;
	case TINYGLTF_COMPONENT_TYPE_INT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		itype = 2;
		break;
	}

	for (int i = 0; i < indcount; i++) {

		unsigned int index = (itype == 0) ? (unsigned int)buff[byteOffset + i] : 
			(itype == 1) ? (unsigned int)*(unsigned short*)&buff[byteOffset + i * 2] : 
			(unsigned int)*(unsigned int*)&buff[byteOffset + i * 4];

		vec3f point = vec3f(getbuff(VERT_OFFSET, 12, 0), getbuff(VERT_OFFSET, 12, 4), getbuff(VERT_OFFSET, 12, 8));
		newmesh.verts[vc++] = point.x;
		newmesh.verts[vc++] = point.y;
		newmesh.verts[vc++] = point.z;
		newmesh.bbox.update(point);

		if (buffOffsets[NORM_OFFSET] > 0) {
			newmesh.verts[vc++] = getbuff(NORM_OFFSET, 12, 0);
			newmesh.verts[vc++] = getbuff(NORM_OFFSET, 12, 4);
			newmesh.verts[vc++] = getbuff(NORM_OFFSET, 12, 8);
		}
		else {
			newmesh.verts[vc++] = 0;
			newmesh.verts[vc++] = 0;
			newmesh.verts[vc++] = 0;
		}
		if (buffOffsets[TEX0_OFFSET] > 0) {
			newmesh.verts[vc++] = getbuff(TEX0_OFFSET, 8, 0);
			newmesh.verts[vc++] = getbuff(TEX0_OFFSET, 8, 4);
		}
		else {
			newmesh.verts[vc++] = 0;
			newmesh.verts[vc++] = 0;
		}

		if (buffOffsets[COL_OFFSET] > 0) {
			uint32_t col = 0;
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
			newmesh.verts[vc++] = Pi3Cutils::colToFloat(col);
		}
		else {
			newmesh.verts[vc++] = white;
		}
	}

	if (buffOffsets[NORM_OFFSET] < 0 && newmesh.mode == GL_TRIANGLES) {
		newmesh.updateNormals(0, newmesh.vc);
	}

	return newmesh;
}

static Pi3Cmodel ParseMesh(Pi3Cresource* resource, tinygltf::Model& model, const tinygltf::Mesh& mesh, const std::vector<int>& texRefs) {

	Pi3Cmodel newmodel;
	Pi3Cmaterial newmaterial;

	auto& buff = model.buffers[0].data;


	for (size_t i = 0; i < mesh.primitives.size(); i++) {

		const tinygltf::Primitive& primitive = mesh.primitives[i];

		if (primitive.indices < 0) return newmodel;

		tinygltf::Material* material = (primitive.material >= 0) ? &model.materials[primitive.material] : nullptr;

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

		Pi3Cmesh basemesh = CreateMesh(resource, model, primitive);
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

				auto& targattr = primitive.targets[targRef];

				std::map<std::string, int>::const_iterator it(targattr.begin());
				std::map<std::string, int>::const_iterator itEnd(targattr.end());
				for (; it != itEnd; it++) {
					std::map<std::string, int>::iterator itf = prim.attributes.find(it->first);
					if (itf != prim.attributes.end()) itf->second = it->second;
				}

				Pi3Cmesh targMesh = CreateMesh(resource, model, prim);
				//Add targMesh verts to base mesh verts
				for (size_t v=0; v < targMesh.vc; v+=targMesh.stride) {
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


static Pi3Cmodel ParseNode(Pi3Cresource* resource, tinygltf::Model& model, const tinygltf::Node& node, const std::vector<int>& texRefs) {
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
			Pi3Cmatrix mtx;
			Pi3Cquaternion quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
			matrix = matrix * quat.toMatrix();
		}

		if (node.translation.size() == 3) {
			matrix.Translate(vec3f(node.translation[0], node.translation[1], node.translation[2]));
		}
	}

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

bool Pi3Cgltf::load(Pi3Cresource* resource, Pi3Cscene* piscene, const std::string& filepath)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	std::string ext = Pi3Cutils::getExt(filepath);
	std::string name = Pi3Cutils::extractName(filepath);

	bool ret = false;

	if (ext == "glb") {
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
	}
	else if (ext == "gltf") {
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
	}

	if (ret) {
		assert(model.scenes.size() > 0);

		std::vector<int> texRefs(0);

		if (model.materials.size() > 0) {
			for (size_t i = 0; i < model.materials.size(); i++) {
				int ti = model.materials[i].pbrMetallicRoughness.baseColorTexture.index;
				if (ti >= 0 && ti < model.images.size()) {
					auto& image = model.images[ti];
					std::string texname = (model.materials[i].name != "") ? model.materials[i].name : filepath + "_tex" + std::to_string(i);
					int32_t ft = resource->findTextureByName(texname);
					if (ft < 0) {
						std::shared_ptr<Pi3Ctexture> Texture;
						int iw = image.width, ih = image.height, sc = 1;
						while (iw > 2048 || ih > 2048) { iw /= 2; ih /= 2; sc *= 2; }
						uint32_t* srcimg = (uint32_t*)&image.image[0];
						Texture.reset(new Pi3Ctexture(iw, ih, image.component));
						uint32_t* tpix = (uint32_t*)Texture->GetTexels();
						if (sc > 1) {
							int q = 0, sp = 0;
							for (int sy = 0; sy < image.height; sy += sc) {
								for (int sx = 0; sx < image.width; sx += sc) {
									int q = sx + sy * image.width;
									tpix[sp++] = srcimg[q];
								}
							}
						}
						else
						{
							memcpy(tpix, srcimg, iw * ih * 4);
						}

						Pi3Cutils::flipImage((uint8_t*)tpix, iw, ih);
						Texture->name = texname;
						ft = resource->addTexture(Texture);
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
		int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
		const tinygltf::Scene& scene = model.scenes[scene_to_display];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			rootmodel.append(resource, ParseNode(resource, model, model.nodes[scene.nodes[i]], texRefs));
		}
		piscene->append3D(rootmodel);
	}

	return ret;
}
