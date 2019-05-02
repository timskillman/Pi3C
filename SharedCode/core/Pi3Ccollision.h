#pragma once
#include "Pi3Cmesh.h"
#include <vector>

namespace Pi3Ccollision {

	float rayIntersect(const vec3f &e1, const vec3f &e2, const vec3f &e3, const vec3f &dir);
	float rayPlaneIntersect(const vec3f &e1, const vec3f &e2, const vec3f &e3, const vec3f &dir);
	float planeIntersect(const vec3f &normal, const vec3f &coord, const vec3f &ray, const vec3f &rayOrigin);
	bool collideVector(const Pi3Cmesh &mesh, const Pi3Cmatrix &mtx, const bool bounce, const vec3f &pos, const vec3f &dir);
	float collideFloor(const Pi3Cmesh &mesh, const Pi3Cmatrix &mtx, const vec3f &pos, float prevHeight);

}