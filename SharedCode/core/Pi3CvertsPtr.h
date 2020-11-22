#pragma once
#include <vector>

struct vertsPtr {
	uint32_t offset;
	std::vector<float> *verts;

	vertsPtr() : offset(0), verts(nullptr) {}
	vertsPtr(uint32_t ptr, std::vector<float> *verts) : offset(ptr), verts(verts) {}
};
