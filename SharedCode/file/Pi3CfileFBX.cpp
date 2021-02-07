#include "Pi3CfileFBX.h"
#include "Pi3CbinStream.h"

#ifdef openFBX
Pi3CopenFBX::Pi3CopenFBX(const std::string& filepath)
{
	Pi3CbinStream b(filepath.c_str());
	std::vector<uint8_t> content(0);
	long file_size = b.readAll(content);
	g_scene = ofbx::load((ofbx::u8*)&content[0], file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
}

void Pi3CopenFBX::importToScene(Pi3Cresource* resource, Pi3Cscene* scene)
{
	int obj_idx = 0;
	int indices_offset = 0;
	int normals_offset = 0;
	int mesh_count = g_scene->getMeshCount();
	float floatCol = Pi3Cutils::colToFloat(0xffffffff);

	size_t startMesh = resource->meshes.size();
	Pi3Cmodel newmodel;

	for (int i = 0; i < mesh_count; ++i)
	{
		const ofbx::Mesh& mesh = *g_scene->getMesh(i);
		const ofbx::Geometry& geom = *mesh.getGeometry();

		const ofbx::Vec3* vertices = geom.getVertices();
		const ofbx::Vec3* normals = geom.getNormals();
		const ofbx::Vec2* uvs = geom.getUVs();

		bool has_uvs = uvs != nullptr;
		bool has_normals = normals != nullptr;

		int vertex_count = geom.getVertexCount();
		int index_count = geom.getIndexCount();

		Pi3Cmesh newmesh;
		newmesh.materialRef = 0;
		std::vector<float>& verts = newmesh.verts;
		uint32_t vc = 0;
		ofbx::Vec3 bn = { 0,0,0 };
		ofbx::Vec2 buv = { 0,0 };

		const int* faceIndices = geom.getFaceIndices();
		for (int i = 0; i < index_count; ++i)
		{
			int idx = (faceIndices[i] < 0) ? -faceIndices[i]-1 : faceIndices[i];

			ofbx::Vec3 v = vertices[idx];
			verts.push_back(v.x);
			verts.push_back(v.y);
			verts.push_back(v.z);
			newmesh.bbox.update(vec3f(v.x, v.y, v.z));

			ofbx::Vec3 n = (has_normals) ? normals[idx] : bn;
			verts.push_back(n.x);
			verts.push_back(n.y);
			verts.push_back(n.z);

			ofbx::Vec2 uv = (has_uvs) ? uvs[idx] : buv;
			verts.push_back(uv.x);
			verts.push_back(uv.y);

			verts.push_back(floatCol);
			newmesh.vc += 9;
		}

		newmodel.appendMeshToGroup(resource, newmesh, 1);
		
	}

	scene->append3D(newmodel);

}
#else
Pi3CopenFBX::Pi3CopenFBX(const std::string& filepath)
{
	//FBXDocument fbxdoc;
	fbxdoc.read(filepath);

	std::ofstream stream("FBX.txt", std::ostream::out | std::ostream::binary);
	//stream = std::ofstream("FBX.txt", std::ostream::out | std::ostream::binary);
	fbxdoc.print(stream);
}

void Pi3CopenFBX::importToScene(Pi3Cresource* resource, Pi3Cscene* scene)
{

}
#endif