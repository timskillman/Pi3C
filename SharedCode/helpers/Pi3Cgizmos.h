#pragma once

#include "Pi3Cmesh.h"

namespace Pi3Cgizmos {
	//void select_box_update(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec3f& size, const uint32_t colint);
	void select_box_verts(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec3f& size, const uint32_t colint);
	Pi3Cmesh selectBoxGizmo(const vec3f &pos, const vec3f &size, const uint32_t col = 0xffffffff);

	Pi3Cmesh moveGizmo(const vec3f &pos, const vec3f &size, const uint32_t col = 0xffffffff);
	Pi3Cmesh scaleGizmo(const vec3f &pos, const vec3f &size, const uint32_t col = 0xffffffff);
	Pi3Cmesh rotateGizmo(const vec3f &pos, const vec3f &size, const uint32_t col = 0xffffffff);
}