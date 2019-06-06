#include "Pi3Ccollision.h"
#include <cmath>

float mypow(const float v) { return v * v; }

bool SphereCollision(const float aradius, vec3f apos, const float bradius, const vec3f bpos)
{
	float r = aradius + bradius;
	r *= r;
	return r < mypow(apos.x + bpos.x) + mypow(apos.y + bpos.y) + mypow(apos.z + bpos.z);
}

float Pi3Ccollision::rayIntersect(const vec3f &e1, const vec3f &e2, const vec3f &s, const vec3f &dir)
{
	/* Based on www.lighthouse3d.com/tutorials/ray-triangle-intersection */
	/* General purpose */

	//h=crossProduct(dir,e2)
	float hx = dir.y * e2.z - e2.y * dir.z;
	float hy = dir.z * e2.x - e2.z * dir.x;
	float hz = dir.x * e2.y - e2.x * dir.y;

	//vec3f n(dir.y * e2.z - e2.y * dir.z, dir.z * e2.x - e2.z * dir.x, dir.x * e2.y - e2.x * dir.y);
	float a = (e1.x * hx + e1.y * hy + e1.z * hz);
	if (a > -0.00001f && a < 0.00001f) return 1e8f; //roughly zero

	float f = 1 / a;
	float u = (s.x * hx + s.y * hy + s.z * hz) * f;
	if (u < 0 || u > 1.f) return 1e8f;

	float qx = s.y * e1.z - e1.y * s.z;
	float qy = s.z * e1.x - e1.z * s.x;
	float qz = s.x * e1.y - e1.x * s.y;
	//vec3f q(e3.y * e1.z - e1.y * e3.z, e3.z * e1.x - e1.z * e3.x, e3.x * e1.y - e1.x * e3.y);
	float v = (dir.x * qx + dir.y * qy + dir.z * qz) * f;
	if (v < 0 || u + v > 1.f) return 1e8f;

	return (e2.x * qx + e2.y * qy + e2.z * qz) * f;  //distance from pos to triangle intersection
}

float Pi3Ccollision::rayPlaneIntersect(const vec3f &e1, const vec3f &e2, const vec3f &e3, const vec3f &dir)
{
	/* Based on www.lighthouse3d.com/tutorials/ray-triangle-intersection */
	/* General purpose */

	vec3f n(dir.y * e2.z - e2.y * dir.z, dir.z * e2.x - e2.z * dir.x, dir.x * e2.y - e2.x * dir.y);
	float a = e1.x * n.x + e1.y * n.y + e1.z * n.z;
	if (a > -0.00001f && a < 0.00001f) return 0.f; //roughly zero

	float f = 1.f / a;
	float u = f * (e3.x * n.x + e3.y * n.y + e3.z * n.z);
	//if (u < 0.f || u > 1.f) return 0.f;

	vec3f q(e3.y * e1.z - e1.y * e3.z, e3.z * e1.x - e1.z * e3.x, e3.x * e1.y - e1.x * e3.y);
	float v = f * (dir.x * q.x + dir.y * q.y + dir.z * q.z);
	//if (v < 0.f || u + v > 1.f) return 0.f;

	return f * (e2.x * q.x + e2.y * q.y + e2.z * q.z);  //distance from pos to triangle intersection
}

float innerProduct(vec3f a, vec3f b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

float Pi3Ccollision::planeIntersect(const vec3f &planeNormal, const vec3f &coord, const vec3f &ray, const vec3f &rayOrigin)
{
	if (innerProduct(planeNormal, ray) == 0) return 0.f;
	float d = innerProduct(planeNormal, coord);
	float x = (d - innerProduct(planeNormal, rayOrigin)) / innerProduct(planeNormal, ray);
	return x;
}

float rayIntersectFloor(const vec3f &e1, const vec3f &e2, const vec3f &e3)
{
	/* Based on www.lighthouse3d.com/tutorials/ray-triangle-intersection */
	/* Optimised with down vector (0,-1,0) */

	float a = e1.x * -e2.z + e1.z * e2.x;
	if (a > -0.00001f && a < 0.00001f) return 1e8f; //roughly zero

	float f = 1.f / a;
	float u = f * (e3.x * -e2.z + e3.z * e2.x);
	if (u < 0.f || u > 1.f) return 1e8f;

	vec3f q(e3.y * e1.z - e1.y * e3.z, e3.z * e1.x - e1.z * e3.x, e3.x * e1.y - e1.x * e3.y);
	float v = f * -q.y;
	if (v < 0.f || u + v > 1.f) return 1e8f;

	return f * (e2.x * q.x + e2.y * q.y + e2.z * q.z);  //distance from pos to triangle intersection
}

bool Pi3Ccollision::collideVector(const Pi3Cresource *resource, const uint32_t meshRef, const Pi3Cmatrix &mtx, const bool bounce, const vec3f &pos, const vec3f &dir)
{
	//if (mesh.verts.size() == 0) return false;
	const Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t p = mesh.vertOffset * mesh.stride;
	const std::vector<float> &verts = resource->vertBuffer[mesh.bufRef];
	uint32_t size = mesh.vertSize * mesh.stride;

	uint32_t stride = mesh.stride;

	vec3f nextpos = pos + dir;
	vec3f v1, v2, v3;
	uint32_t x2 = stride;
	uint32_t x3 = stride * 2;
	uint32_t st = stride * 3;

	for (size_t sp = p; sp < p + size - st; sp += st) {

		v1 = mtx.transformVec(&verts[sp]);
		v2 = mtx.transformVec(&verts[sp + x2]);
		v3 = mtx.transformVec(&verts[sp + x3]);

		float d1 = rayIntersect(v2 - v1, v3 - v1, pos - v1, dir);

		if ((d1 > 0.f && d1 < 1.f)) {
			//pos = pos + (dir * d1);   //return bounce angle and direction
			//dir = tri.reflection(dir);
			return true;
		}
	}

	return false;
}

float Pi3Ccollision::collideFloor(const Pi3Cresource *resource, const uint32_t meshRef, const Pi3Cmatrix &mtx, const vec3f &pos, float prevHeight)
{
	const Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t p = mesh.vertOffset * mesh.stride;
	const std::vector<float> &verts = resource->vertBuffer[mesh.bufRef];
	uint32_t size = mesh.vertSize * mesh.stride;

	//if (mesh.verts.size() == 0) return prevHeight;

	//check if transformed bounding box 'footprint' collides with pos (sphere collision better?) ...
	if (!mesh.bbox.pointInsideXZtx(pos, mtx)) return prevHeight;

	//const std::vector<float> &verts = mesh.verts;
	uint32_t stride = mesh.stride;

	vec3f v1, v2, v3;
	uint32_t x2 = stride; uint32_t z2 = stride + 2;
	uint32_t x3 = stride * 2; uint32_t z3 = stride * 2 + 2;

	for (size_t sp = p; sp < p + size - stride * 3; sp += stride * 3) {

		v1 = mtx.transformVec(&verts[sp]);
		v2 = mtx.transformVec(&verts[sp + x2]);
		v3 = mtx.transformVec(&verts[sp + x3]);

		// Optimised bounds check (no type conversions) ...
		if (v1.x > pos.x && v2.x > pos.x && v3.x > pos.x) continue;
		if (v1.x < pos.x && v2.x < pos.x && v3.x < pos.x) continue;
		if (v1.z > pos.z && v2.z > pos.z && v3.z > pos.z) continue;
		if (v1.z < pos.z && v2.z < pos.z && v3.z < pos.z) continue;

		// OK - ray is in triangle footprint - check intersection...
		float d1 = rayIntersectFloor(v2 - v1, v3 - v1, pos - v1);
		if (d1 > 0 && d1 < prevHeight) 
			prevHeight = d1;
	}

	return prevHeight;
}
