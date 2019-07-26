#include "Pi3Cgizmos.h"

namespace Pi3Cgizmos {

	void storeVC(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const float col)
	{
		/* Either creates new vertices or updates them */
		if (vc + 9 > verts.size()) verts.resize(vc + 9);
		verts[vc++] = pos.x;
		verts[vc++] = pos.y;
		verts[vc++] = pos.z;
		verts[vc++] = 0;
		verts[vc++] = 1.f;
		verts[vc++] = 0;
		verts[vc++] = 0;
		verts[vc++] = 0;
		verts[vc++] = col;
	}

	void updateVerts(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const float col)
	{
		/* Either creates new vertices or updates them */
		verts[vc] = pos.x;
		verts[vc+1] = pos.y;
		verts[vc+2] = pos.z;
		verts[vc+8] = col;
		vc += 9;
	}

	void select_box_verts(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec3f& size, const uint32_t colint)
	{
		/* Vertice 'trace' over each other as there is no move .. 144 bytes*/
		float col = (float)(colint & 255) / 256.f + (float)((colint >> 8) & 255) + (float)((colint >> 16) & 255) * 256.f;
		storeVC(verts, vc, pos + vec3f(0, 0, 0), col);
		storeVC(verts, vc, pos + vec3f(size.x, 0, 0), col);
		storeVC(verts, vc, pos + vec3f(size.x, size.y, 0), col);
		storeVC(verts, vc, pos + vec3f(0, size.y, 0), col);
		storeVC(verts, vc, pos + vec3f(0, 0, 0), col);
		storeVC(verts, vc, pos + vec3f(0, 0, size.z), col);
		storeVC(verts, vc, pos + vec3f(size.x, 0, size.z), col);
		storeVC(verts, vc, pos + vec3f(size.x, size.y, size.z), col);
		storeVC(verts, vc, pos + vec3f(0, size.y, size.z), col);
		storeVC(verts, vc, pos + vec3f(0, 0, size.z), col);
		storeVC(verts, vc, pos + vec3f(0, size.y, size.z), col);
		storeVC(verts, vc, pos + vec3f(0, size.y, 0), col);
		storeVC(verts, vc, pos + vec3f(size.x, size.y, 0), col);
		storeVC(verts, vc, pos + vec3f(size.x, size.y, size.z), col);
		storeVC(verts, vc, pos + vec3f(size.x, 0, size.z), col);
		storeVC(verts, vc, pos + vec3f(size.x, 0, 0), col);
	}

	void select_box_update(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec3f& size, const uint32_t colint)
	{
		/* Vertice 'trace' over each other as there is no move .. 144 bytes*/
		float col = (float)(colint & 255) / 256.f + (float)((colint >> 8) & 255) + (float)((colint >> 16) & 255) * 256.f;
		updateVerts(verts, vc, pos + vec3f(0, 0, 0), col);
		updateVerts(verts, vc, pos + vec3f(size.x, 0, 0), col);
		updateVerts(verts, vc, pos + vec3f(size.x, size.y, 0), col);
		updateVerts(verts, vc, pos + vec3f(0, size.y, 0), col);
		updateVerts(verts, vc, pos + vec3f(0, 0, 0), col);
		updateVerts(verts, vc, pos + vec3f(0, 0, size.z), col);
		updateVerts(verts, vc, pos + vec3f(size.x, 0, size.z), col);
		updateVerts(verts, vc, pos + vec3f(size.x, size.y, size.z), col);
		updateVerts(verts, vc, pos + vec3f(0, size.y, size.z), col);
		updateVerts(verts, vc, pos + vec3f(0, 0, size.z), col);
		updateVerts(verts, vc, pos + vec3f(0, size.y, size.z), col);
		updateVerts(verts, vc, pos + vec3f(0, size.y, 0), col);
		updateVerts(verts, vc, pos + vec3f(size.x, size.y, 0), col);
		updateVerts(verts, vc, pos + vec3f(size.x, size.y, size.z), col);
		updateVerts(verts, vc, pos + vec3f(size.x, 0, size.z), col);
		updateVerts(verts, vc, pos + vec3f(size.x, 0, 0), col);
	}


	Pi3Cmesh selectBoxGizmo(const vec3f &pos, const vec3f &size, const uint32_t col) 
	{
		Pi3Cmesh mesh("selectBox");
		mesh.stride = 9;

		select_box_verts(mesh.verts, mesh.vc, pos, size, col);

		mesh.bbox.bboxFromVerts(mesh.verts, 0, mesh.vc, mesh.stride);
		mesh.materialRef = 0;
		return mesh;
	}

	void moveGixmo_verts(std::vector<float>& verts, uint32_t& vc, const float size)
	{
		uint32_t colint = 0xff;
		float col = (float)(colint & 255) / 256.f + (float)((colint >> 8) & 255) + (float)((colint >> 16) & 255) * 256.f;
		storeVC(verts, vc, vec3f(0, 0, 0), col);
		storeVC(verts, vc, vec3f(size, 0, 0), col);
		colint = 0xff00;
		col = (float)(colint & 255) / 256.f + (float)((colint >> 8) & 255) + (float)((colint >> 16) & 255) * 256.f;
		storeVC(verts, vc, vec3f(0, 0, 0), col);
		storeVC(verts, vc, vec3f(0, size, 0), col);
		colint = 0xff0000;
		col = (float)(colint & 255) / 256.f + (float)((colint >> 8) & 255) + (float)((colint >> 16) & 255) * 256.f;
		storeVC(verts, vc, vec3f(0, 0, 0), col);
		storeVC(verts, vc, vec3f(0, 0, size), col);
	}

	Pi3Cmesh moveGizmo(const vec3f &pos, const vec3f &size, const uint32_t col)
	{
		Pi3Cmesh mesh("moveGizmo");
		mesh.stride = 9;

		moveGixmo_verts(mesh.verts, mesh.vc, 1.f);

		mesh.bbox.bboxFromVerts(mesh.verts, 0, mesh.vc, mesh.stride);
		mesh.materialRef = 0;
		return mesh;
	}

	Pi3Cmesh scaleGizmo(const vec3f &pos, const vec3f &size, const uint32_t col)
	{
		Pi3Cmesh mesh("scaleGizmo");
		mesh.stride = 9;

		mesh.bbox.bboxFromVerts(mesh.verts, 0, mesh.vc, mesh.stride);
		mesh.materialRef = 0;
		return mesh;
	}

	Pi3Cmesh rotateGizmo(const vec3f &pos, const vec3f &size, const uint32_t col)
	{
		Pi3Cmesh mesh("rotateGizmo");
		mesh.stride = 9;

		mesh.bbox.bboxFromVerts(mesh.verts, 0, mesh.vc, mesh.stride);
		mesh.materialRef = 0;
		return mesh;
	}
}