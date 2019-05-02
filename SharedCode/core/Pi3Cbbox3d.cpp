#include "Pi3Cbbox3d.h"
#ifdef __WINDOWS__
#include <cmath>
#endif

void Pi3Cbbox3d::init()
{
	min = vec3f(1e8, 1e8, 1e8);
	max = vec3f(-1e8, -1e8, -1e8);
	radius = 0;
}

Pi3Cbbox3d::Pi3Cbbox3d(const vec3f min, const vec3f max)
{
	this->min = min;
	this->max = max;
	radius = calcradius();
}

void Pi3Cbbox3d::update(const vec3f &point) 
{
	if (point.x < min.x) min.x = point.x;
	if (point.x > max.x) max.x = point.x;
	if (point.y < min.y) min.y = point.y;
	if (point.y > max.y) max.y = point.y;
	if (point.z < min.z) min.z = point.z;
	if (point.z > max.z) max.z = point.z;
	radius = calcradius();
}

void Pi3Cbbox3d::update(const Pi3Cbbox3d &box, const Pi3Cmatrix* mtx)
{
	update(mtx->transformVec(vec3f(box.min.x, box.min.y, box.min.z)));
	update(mtx->transformVec(vec3f(box.max.x, box.min.y, box.min.z)));
	update(mtx->transformVec(vec3f(box.max.x, box.max.y, box.min.z)));
	update(mtx->transformVec(vec3f(box.min.x, box.max.y, box.min.z)));
	update(mtx->transformVec(vec3f(box.min.x, box.min.y, box.max.z)));
	update(mtx->transformVec(vec3f(box.max.x, box.min.y, box.max.z)));
	update(mtx->transformVec(vec3f(box.max.x, box.max.y, box.max.z)));
	update(mtx->transformVec(vec3f(box.min.x, box.max.y, box.max.z)));
}

void Pi3Cbbox3d::update(const Pi3Cbbox3d &box)
{
	update(vec3f(box.min.x, box.min.y, box.min.z));
	update(vec3f(box.max.x, box.min.y, box.min.z));
	update(vec3f(box.max.x, box.max.y, box.min.z));
	update(vec3f(box.min.x, box.max.y, box.min.z));
	update(vec3f(box.min.x, box.min.y, box.max.z));
	update(vec3f(box.max.x, box.min.y, box.max.z));
	update(vec3f(box.max.x, box.max.y, box.max.z));
	update(vec3f(box.min.x, box.max.y, box.max.z));
}

void Pi3Cbbox3d::set(const Pi3Cbbox3d &box, const Pi3Cmatrix* mtx)
{
	init();
	update(box, mtx);
}

void Pi3Cbbox3d::bboxFromVerts(const std::vector<float> &verts, const uint32_t start, const uint32_t vsize, const uint32_t stride)
{
	min.x = verts[start]; max.x = min.x;
	min.y = verts[start + 1]; max.y = min.y;
	min.z = verts[start + 2]; max.z = min.z;

	for (uint32_t i = 0; i < vsize; i += stride) {
		if (verts[i] < min.x) min.x = verts[i]; else if (verts[i] > max.x) max.x = verts[i];
		if (verts[i + 1] < min.y) min.y = verts[i + 1]; else if (verts[i + 1] > max.y) max.y = verts[i + 1];
		if (verts[i + 2] < min.z) min.z = verts[i + 2]; else if (verts[i + 2] > max.z) max.z = verts[i + 2];
	}
}

void Pi3Cbbox3d::radiusFromVerts(const std::vector<float> &verts, const vec3f centre, const uint32_t start, const uint32_t size, const uint32_t stride)
{
	uint32_t st = start*stride;
	uint32_t sz = size*stride;
	//Works out the bounding radius of the vertices
	radius = 0;
	//TODO: This should strictly determine bounds by vertices 'used' in the scene according to polygon indices.
	for (std::size_t i = st; i < (st+sz); i=stride) {
		vec3f d = vec3f(verts[i], verts[i+1], verts[i+2]) - centre;
		float r = sqrtf(d.x*d.x + d.y*d.y + d.z*d.z);
		if (r > radius) radius = r;
	}
}

float Pi3Cbbox3d::radiusFromTVerts(const std::vector<float> &verts, const vec3f centre, const Pi3Cmatrix* mtx, const uint32_t start, const uint32_t size, const uint32_t stride) const
{
	uint32_t st = start*stride;
	uint32_t sz = size*stride;
	//Works out the bounding radius of the vertices
	float rad = 0;
	//TODO: This should strictly determine bounds by vertices 'used' in the scene according to polygon indices.
	for (std::size_t i = st; i < (st + sz); i = stride) {
		vec3f d = mtx->transformVec(vec3f(verts[i], verts[i + 1], verts[i + 2])) - centre;
		float r = sqrtf(d.x*d.x + d.y*d.y + d.z*d.z);
		if (r > rad) rad = r;
	}
	return rad;
}

Pi3Cbbox3d Pi3Cbbox3d::bboxFromTVerts(const Pi3Cmatrix* mtx) const
{
	//Transforms a bounding box with a matrix
	Pi3Cbbox3d tbox;
	tbox.update(*this);
	return tbox;
}

void Pi3Cbbox3d::bboxFromTVerts(const std::vector<float> &verts, const Pi3Cmatrix* mtx, const uint32_t start, const uint32_t size, const uint32_t stride)
{
	uint32_t st = start*stride;
	uint32_t sz = size*stride;

	//Assumes first three values of the vertex element is position data
	min = mtx->transformVec(vec3f(verts[st], verts[st+1], verts[st+2]));
	max = min;

	//TODO: This should strictly determine bounds by vertices 'used' in the scene according to polygon indices.
	for (std::size_t i = st; i < (st+sz); i += stride) {
		vec3f tvec = mtx->transformVec(vec3f(verts[i], verts[i + 1], verts[i + 2]));
		if (tvec.x < min.x) min.x = tvec.x; else if (tvec.x > max.x) max.x = tvec.x;
		if (tvec.y < min.y) min.y = tvec.y; else if (tvec.y > max.y) max.y = tvec.y;
		if (tvec.z < min.z) min.z = tvec.z; else if (tvec.z > max.z) max.z = tvec.z;
	}

	radius = radiusFromTVerts(verts, center(), mtx, start, size, stride);
}

void Pi3Cbbox3d::translate(const vec3f t)
{
	min += t; max += t;
}

void Pi3Cbbox3d::moveto(const vec3f p)
{
	vec3f bc = center();
	min = (min-bc)+p; 
	max = (max-bc)+p;
}