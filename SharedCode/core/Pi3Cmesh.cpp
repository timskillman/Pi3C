#include "Pi3Cmesh.h"
#include "Pi3Ccollision.h"

Pi3Cmesh::Pi3Cmesh(const std::string &name, const uint32_t stride) : name(name), stride(stride) {
	reset();
}

void Pi3Cmesh::reset()
{
	mode = GL_TRIANGLES; 
	vertOffset = 0; 
	vertSize = 0;
	verts.resize(0);
	bbox.init();
	materialRef = -1;
	vc = 0;
	stride = 9;
}

void Pi3Cmesh::updateBounds(const vertsPtr* vp)
{
	if (vp==nullptr)
		bbox.bboxFromVerts(verts, 0, vc, stride);
	else
		bbox.bboxFromVerts(*vp->verts, vp->offset, vc, stride);
}

void Pi3Cmesh::addPackedVert(const vec3f &position, const vec3f &normal, const vec2f &uv, const uint32_t col)
{
	if (vc + stride > verts.size()) verts.resize(vc + 1000);
	verts[vc++] = position.x;
	verts[vc++] = position.y;
	verts[vc++] = position.z;
	verts[vc++] = normal.x;
	verts[vc++] = normal.y;
	verts[vc++] = normal.z;
	verts[vc++] = uv.x;
	verts[vc++] = uv.y;
	verts[vc++] = (float)(col & 255) / 256.f + (float)((col >> 8) & 255) + (float)((col >> 16) & 255) * 256.f; // Pack 32bit colour in float (65535.996f = 0xffffff)
	//if (cols && stride==12) {
	//	//verts[vc++] = (colour & 255) + ((colour >> 8) && 255) * 256 + ((colour >> 16) && 255) * 65536;
	//	verts[vc++] = cols[0];
	//	verts[vc++] = cols[1];
	//	verts[vc++] = cols[2];
	//	verts[vc++] = cols[3];
	//}
}

void Pi3Cmesh::offset(const vec3f pos, std::vector<float> &verts, const uint32_t start, const uint32_t size, const uint32_t stride)
{
	std::size_t st = start*stride;
	std::size_t sz = size*stride;
	
	if (sz > 3) {
		for (std::size_t i = st; i < (st + sz); i += stride) {
			verts[i] += pos.x; 
			verts[i + 1] += pos.y; 
			verts[i + 2] += pos.z;
		}
	}
}

void Pi3Cmesh::resize(const vec3f &pos, const vec3f &size, const std::vector<float> &mverts)
{
	uint32_t vsize = vertSize * stride;
	uint32_t voff = vertOffset * stride;
	
	vec3f scale(size.x / bbox.width(), size.y / bbox.height(), size.z / bbox.depth());
	vec3f center = bbox.center();

	for (size_t i = voff; i < (voff + vsize); i += stride * 3)
	{
		verts[i] = (verts[i] - center.x)*scale.x + pos.x;
		verts[i + 1] = (verts[i + 1] - center.y)*scale.y + pos.y;
		verts[i + 2] = (verts[i + 2] - center.z)*scale.z + pos.z;
	}
}

Pi3Cmesh Pi3Cmesh::clone(const uint32_t from, const uint32_t size) const
{
	/* we only want to partially clone some of the vertices */
	Pi3Cmesh newmesh = *this;
	newmesh.name = name;
	newmesh.bbox = bbox;
	newmesh.mode = mode;
	newmesh.vertOffset = vertOffset;
	newmesh.vertSize = vertSize;
	newmesh.stride = stride;
	newmesh.materialRef = materialRef;
	newmesh.vc = vc;
	newmesh.bufRef = bufRef;
	//newmesh.dynamic = dynamic;
	if (size > 0) {
		if (size == verts.size()) {
			newmesh.verts = verts;
		}
		else {
			newmesh.vertSize = size;
			newmesh.verts.resize(size);
			memcpy(newmesh.verts.data(), &verts[from], size * sizeof(float));
		}
	}
	return newmesh;
}

void Pi3Cmesh::render(const GLenum rendermode) {
	glDrawArrays(rendermode, vertOffset, vertSize);
}

void Pi3Cmesh::renderIndexed(const GLenum rendermode, uint32_t indexCo, uint32_t * indexes)
{
	glDrawElements(rendermode, indexCo, GL_UNSIGNED_INT, indexes);
}

//vec2f point3Dto2D(const vec3f vec, const float psp) const
//{
//	float zd = psp / vec.z;
//	return vec2f(vec.x*zd, vec.y*zd);
//}

int32_t Pi3Cmesh::touchPoint(Pi3Ctouch &touch, const Pi3Cmatrix &mtx, const std::vector<float> &mverts) const
{
	/* Screen x,y touch is acheived by transforming each mesh triangle into 2D screen space,
	   perform various 2D checks and then check if the equivalent transformed touch point lies within 
	   the 3D triangle in screen space.

	   2D checks (such as bounds checking and back-face elimination) are faster than 3D checks
	   and discard a triangle at the earliest opportunity to avoid unnecessary calculations.
	*/

	//if (vc == 0 || verts.size() == 0) return -1;

	uint32_t vsize = vertSize * stride;
	uint32_t voff = vertOffset * stride;
	int32_t closestTri = -1;
	float w1 = 1.f, w2 = 1.f, w3 = 1.f;

	for (size_t i = voff; i < (voff + vsize); i += stride * 3)
	{
		uint32_t i2 = i + stride;
		uint32_t i3 = i + stride + stride;
		bool touchPsp = touch.perspective != 0;

		// Calc Z's and discard any triangle that is behind the viewer ...
		int crossViewer = 0;
		float z1 = mtx.transformZ(&mverts[i]); if (touchPsp && z1 > 0) crossViewer = crossViewer | 1;
		float z2 = mtx.transformZ(&mverts[i2]); if (touchPsp && z2 > 0) crossViewer = crossViewer | 2;
		float z3 = mtx.transformZ(&mverts[i3]); if (touchPsp && z3 > 0) crossViewer = crossViewer | 4;
		if (crossViewer == 7) continue;

		if (crossViewer != 0) {
			//Triangle needs clipping or splitting into two triangles...
			crossViewer = crossViewer;
		}

		if (touchPsp) {
			//if (z1 > 0 && z2 > 0 && z3 > 0) continue;
			w1 = touch.perspective / z1, w2 = touch.perspective / z2, w3 = touch.perspective / z3;
		}

		float x1 = mtx.transformX(&mverts[i]);
		float x2 = mtx.transformX(&mverts[i2]);
		float x3 = mtx.transformX(&mverts[i3]);

		// discard triangle if touch point is left or right of the triangle ...
		float xw1 = x1 * w1, xw2 = x2 * w2, xw3 = x3 * w3;
		if (xw1 > touch.touchPoint.x && xw2 > touch.touchPoint.x && xw3 > touch.touchPoint.x) continue;
		if (xw1 < touch.touchPoint.x && xw2 < touch.touchPoint.x && xw3 < touch.touchPoint.x) continue;

		float y1 = mtx.transformY(&mverts[i]);
		float y2 = mtx.transformY(&mverts[i2]);
		float y3 = mtx.transformY(&mverts[i3]);

		// discard triangle if touch point is above or below the triangle ...
		float yw1 = y1 * w1, yw2 = y2 * w2, yw3 = y3 * w3;
		if (yw1 > touch.touchPoint.y && yw2 > touch.touchPoint.y && yw3 > touch.touchPoint.y) continue;
		if (yw1 < touch.touchPoint.y && yw2 < touch.touchPoint.y && yw3 < touch.touchPoint.y) continue;

		// check if triangle is back-facing and if so, discard ...
		if ((-yw2 * xw3 + yw1 * (-xw2 + xw3) + xw1 * (yw2 - yw3) + xw2 * yw3) < 0) continue;

		//we've found the triangle in 2D, now find the true 3D intersection (if it exists) in world space ...
		vec3f v1(x2 - x1, y2 - y1, z2 - z1);
		vec3f v2(x3 - x1, y3 - y1, z3 - z1);
		vec3f v3(touch.raypos.x - x1, touch.raypos.y - y1, touch.raypos.z - z1);

		float dist = Pi3Ccollision::rayIntersect(v1, v2, v3, touch.raydir);

		if (dist < touch.prevDist) {
			closestTri = i;
			vec3f scaledRay = touch.raydir * dist;
			touch.intersection = touch.raypos + scaledRay;
			touch.normal = v1.trinormal(v2,v3);				//triangle normal
			touch.prevDist = dist;
		}
	}

	return closestTri;
}

namespace std
{
	//Map for vertex x,y,z coords ...

	struct mapvert {
		mapvert(const float x, const float y, const float z) : x(x), y(y), z(z) {}
		friend bool operator== (const mapvert &a, const mapvert &b);

		uint32_t index;
		float x, y, z;
	};

	bool operator== (const mapvert &a, const mapvert &b) {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	template <>
	struct hash<mapvert>
	{
		size_t operator()(const mapvert &v) const {
			return hash<float>()(v.x) ^ hash<float>()(v.y) ^ hash<float>()(v.z);
		}
	};

	//Map for texture u,v coords ...

	struct mapuv {
		mapuv(const float u, const float v) : u(u), v(v) {}
		friend bool operator== (const mapuv &a, const mapuv &b);

		uint32_t index;
		float u, v;
	};

	bool operator== (const mapuv &a, const mapuv &b) {
		return a.u == b.u && a.v == b.v;
	}

	template <>
	struct hash<mapuv>
	{
		size_t operator()(const mapuv &v) const {
			return hash<float>()(v.u) ^ hash<float>()(v.v);
		}
	};

	//Map for triangle edge indices ...

	struct mapedges {
		mapedges(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2) : x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2) {}
		friend bool operator== (const mapedges &a, const mapedges &b);

		uint32_t index;
		float x1, y1, z1, x2, y2, z2;
	};

	bool operator== (const mapedges &a, const mapedges &b) { //check verts in both directions ...
		return (a.x1 == b.x1 && a.y1 == b.y1 && a.z1 == b.z1 && a.x2 == b.x2 && a.y2 == b.y2 && a.z2 == b.z2) || 
			(a.x1 == b.x2 && a.y1 == b.y2 && a.z1 == b.z2 && a.x2 == b.x1 && a.y2 == b.y1 && a.z2 == b.z1);
	}

	template <>
	struct hash<mapedges>
	{
		size_t operator()(const mapedges &v) const {
			return hash<float>()(v.x1) ^ hash<float>()(v.y1) ^ hash<float>()(v.z1) ^ hash<float>()(v.x2) ^ hash<float>()(v.y2) ^ hash<float>()(v.z2);
		}
	};
}

void Pi3Cmesh::createSharedTriangleList(const std::vector<float> &mverts, 
	std::vector<uint32_t> &vertindexes, std::vector<float> &newverts, 
	std::vector<uint32_t> &normindexes, std::vector<float> &newnorms,
	std::vector<uint32_t> &uvindexes, std::vector<float> &newuvs, const float tolerance)
{
	//From a soup of triangles, work out which triangles have matching vertices and count them as 'shared'

	std::unordered_set<std::mapvert> mapverts;

	float itol = 1.f / tolerance;
	uint32_t index = 0;
	uint32_t voff = vertOffset * stride;

	for (size_t i = voff; i < (voff + vc); i += stride) {
		float x = ((float)((int)(mverts[i] * itol))) * tolerance;
		float y = ((float)((int)(mverts[i + 1] * itol))) * tolerance;
		float z = ((float)((int)(mverts[i + 2] * itol))) * tolerance;
		std::mapvert v(x, y, z);
		auto it = mapverts.find(v);
		if (it == mapverts.end()) {
			vertindexes.push_back(index);
			v.index = index++;
			mapverts.insert(v);
		}
		else {
			vertindexes.push_back(it->index);
		}
	}

	newverts.resize(mapverts.size() * 3);
	for (auto it = mapverts.begin(); it != mapverts.end(); ++it) {
		uint32_t t = it->index * 3;
		newverts[t] = it->x; newverts[t + 1] = it->y; newverts[t + 2] = it->z;
	}


	//Norms ...
	std::unordered_set<std::mapvert> mapnorms;

	index = 0;
	for (size_t i = voff; i < (voff + vc); i += stride) {
		float x = ((float)((int)(mverts[i + 3] * itol))) * tolerance;
		float y = ((float)((int)(mverts[i + 4] * itol))) * tolerance;
		float z = ((float)((int)(mverts[i + 5] * itol))) * tolerance;
		std::mapvert v(x, y, z);
		auto it = mapnorms.find(v);
		if (it == mapnorms.end()) {
			normindexes.push_back(index);
			v.index = index++;
			mapnorms.insert(v);
		}
		else {
			normindexes.push_back(it->index);
		}
	}

	newnorms.resize(mapnorms.size() * 3);
	for (auto it = mapnorms.begin(); it != mapnorms.end(); ++it) {
		uint32_t t = it->index * 3;
		newnorms[t] = it->x; newnorms[t + 1] = it->y; newnorms[t + 2] = it->z;
	}

	std::unordered_set<std::mapuv> mapuvs;

	index = 0;
	for (size_t i = voff; i < (voff + vc); i += stride) {
		float x = ((float)((int)(mverts[i + 6] * itol))) * tolerance;
		float y = ((float)((int)(mverts[i + 7] * itol))) * tolerance;
		std::mapuv v(x, y);

		auto it = mapuvs.find(v);
		if (it == mapuvs.end()) {
			//newuvs.push_back(mverts[i + 6]);
			//newuvs.push_back(mverts[i + 7]);
			uvindexes.push_back(index);
			v.index = index++;
			mapuvs.insert(v);
		}
		else {
			uvindexes.push_back(it->index);
		}
	}

	newuvs.resize(mapuvs.size() * 2);
	for (auto it = mapuvs.begin(); it != mapuvs.end(); ++it) {
		uint32_t t = it->index * 2;
		newuvs[t] = it->u; newuvs[t + 1] = it->v;
	}
}

void Pi3Cmesh::createSharedTriangleList(VertsIndsUVs *in, VertsIndsUVs *out, const float tolerance)
{
	//From a soup of triangles, work out which triangles have matching vertices and count them as 'shared'

	std::unordered_set<std::mapvert> mapverts;

	float itol = 1.f / tolerance;
	uint32_t index = 0;
	for (size_t i = vertOffset; i < (vertOffset + vc); i += stride) {
		float x = ((float)((int)(in->verts[i] * itol))) * tolerance;
		float y = ((float)((int)(in->verts[i + 1] * itol))) * tolerance;
		float z = ((float)((int)(in->verts[i + 2] * itol))) * tolerance;
		std::mapvert v(x, y, z);
		auto it = mapverts.find(v);
		if (it == mapverts.end()) {
			out->indexes.push_back(index);
			v.index = index++;
			mapverts.insert(v);
		}
		else {
			out->indexes.push_back(it->index);
		}
	}

	out->verts.resize(mapverts.size() * 3);
	for (auto it = mapverts.begin(); it != mapverts.end(); ++it) {
		uint32_t t = it->index * 3;
		out->verts[t] = it->x; out->verts[t + 1] = it->y; out->verts[t + 2] = it->z;
	}

	std::unordered_set<std::mapuv> mapuvs;

	index = 0;
	for (size_t i = 0; i < vc; i += stride) {
		std::mapuv v(in->verts[i + 6], in->verts[i + 7]);
		auto it = mapuvs.find(v);
		if (it == mapuvs.end()) {
			out->uvs.push_back(in->verts[i + 6]);
			out->uvs.push_back(in->verts[i + 7]);
			out->uvindexes.push_back(index);
			v.index = index++;
			mapuvs.insert(v);
		}
		else {
			out->uvindexes.push_back(it->index);
		}
	}
}

void checkEdge(std::unordered_set<std::mapedges>& mapEdges, std::vector<float> &verts, std::vector<uint32_t>& lineIndexes, uint32_t stride, size_t v1, size_t v2)
{
	uint32_t a = v1 * stride; 
	uint32_t b = v2 * stride;
	std::mapedges edge(verts[a], verts[a + 1], verts[a + 2], verts[b], verts[b + 1], verts[b + 2]);
	auto it = mapEdges.find(edge);
	if (it == mapEdges.end()) {
		lineIndexes.push_back(v1);
		lineIndexes.push_back(v2);
		mapEdges.insert(edge);
	}
}

void Pi3Cmesh::createTriangleEdges(std::vector<float> &verts)
{
	std::unordered_set<std::mapedges> mapedges;
	lineIndexes.clear();

	for (size_t i = vertOffset; i < (vertOffset + vertSize); i += 3) {
		checkEdge(mapedges, verts, lineIndexes, stride, i, i + 1);
		checkEdge(mapedges, verts, lineIndexes, stride, i + 1, i + 2);
		checkEdge(mapedges, verts, lineIndexes, stride, i + 2, i);
	}
}

float Pi3Cmesh::triArea(const vec3f &v1, const vec3f &v2, const vec3f &v3, float &maxLength) const
{
	//work out distances between points ...
	float a = sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
	float b = sqrtf((v2.x - v3.x)*(v2.x - v3.x) + (v2.y - v3.y)*(v2.y - v3.y) + (v2.z - v3.z)*(v2.z - v3.z));
	float c = sqrtf((v3.x - v1.x)*(v3.x - v1.x) + (v3.y - v1.y)*(v3.y - v1.y) + (v3.z - v1.z)*(v3.z - v1.z));

	//apply Heron's formula ...
	float s = (a + b + c) * 0.5f; //s = half perimeter value
	float A = (s*(s - a)*(s - b)*(s - c)) * 0.5f;

	maxLength = (a>b) ? a:b;
	if (c > maxLength) maxLength = c;

	return A;
}

void Pi3Cmesh::updateNormals(const uint32_t min, const uint32_t max)
{
	uint32_t newmax = (max == 0 || max> vc) ? vc : max;

	vec3f v0,v1,v2;
	uint32_t x2 = stride;
	uint32_t x3 = stride * 2;
	//size_t sf = 3 * sizeof(float);

	for (size_t i = min; i < newmax; i += stride*3) {
		memcpy(&v0, &verts[i], sizeof(vec3f));
		memcpy(&v1, &verts[i + x2], sizeof(vec3f));
		memcpy(&v2, &verts[i + x3], sizeof(vec3f));
		vec3f n = -((v0 - v1) ^ (v1 - v2)).unit();
		memcpy(&verts[i + 3], &n, sizeof(vec3f));
		memcpy(&verts[i + x2 + 3], &n, sizeof(vec3f));
		memcpy(&verts[i + x3 + 3], &n, sizeof(vec3f));
	}
}

//void Pi3Cmesh::createOutlines()
//{
//	/* Create outline formed from 3x single lines per triangle (not very efficient!) */
//	overts.resize(verts.size() * 2);
//	uint32_t o = 0;
//	for (size_t v = 0; v < verts.size(); v += 3 * stride) {
//		memcpy(&overts[0] + o, &verts[0] + v, stride); o += stride;
//		memcpy(&overts[0] + o, &verts[0] + v + stride, stride); o += stride;
//
//		memcpy(&overts[0] + o, &verts[0] + v + stride, stride); o += stride;
//		memcpy(&overts[0] + o, &verts[0] + v + stride * 2, stride); o += stride;
//
//		memcpy(&overts[0] + o, &verts[0] + v + stride * 2, stride); o += stride;
//		memcpy(&overts[0] + o, &verts[0] + v, stride); o += stride;
//	}
//}

#define CreateVertsXY(x,y,ux,uy)								\
		verts[vc++] = x; verts[vc++] = y; verts[vc++] = pos.z;	\
		verts[vc++] = 0; verts[vc++] = -1.f; verts[vc++] = 0;	\
		verts[vc++] = ux; verts[vc++] = 0.9999f-uy;				\
		verts[vc++] = 65535.996f;								\

void Pi3Cmesh::addRect(const vec3f &pos, const vec2f &size, const vec2f &uv, const vec2f &uvsize)
{
	if (vc + stride * 6 > verts.size()) verts.resize(vc + stride * 6);
	CreateVertsXY(pos.x, pos.y - size.y, uv.x, uv.y - uvsize.y);
	CreateVertsXY(pos.x + size.x, pos.y - size.y, uv.x + uvsize.x, uv.y - uvsize.y);
	CreateVertsXY(pos.x + size.x, pos.y , uv.x + uvsize.x, uv.y);
	CreateVertsXY(pos.x, pos.y - size.y, uv.x, uv.y - uvsize.y);
	CreateVertsXY(pos.x + size.x, pos.y, uv.x + uvsize.x, uv.y);
	CreateVertsXY(pos.x, pos.y , uv.x, uv.y);

	vertSize = vc / stride;
	bbox.update(vec3f(pos.x, pos.y, pos.z));
	bbox.update(vec3f(pos.x + size.x, pos.y + size.y, pos.z));
}

#define CreateVertsYZ(z,y,ux,uy)									\
		verts[vc++] = pos.x; verts[vc++] = y; verts[vc++] = z;	\
		verts[vc++] = 1.f; verts[vc++] = 0; verts[vc++] = 0;	\
		verts[vc++] = ux; verts[vc++] = 0.9999f-uy;				\
		verts[vc++] = 65535.996f;								\

void Pi3Cmesh::addXshape(const vec3f& pos, const vec2f& size, const vec2f& uv, const vec2f& us)
{
	if (vc + stride * 12 > verts.size()) verts.resize(vc + stride * 12);
	float hx = size.x / 2.f;

	CreateVertsXY(pos.x - hx, pos.y - size.y, uv.x, uv.y - us.y);
	CreateVertsXY(pos.x + hx, pos.y, uv.x + us.x, uv.y);
	CreateVertsXY(pos.x + hx, pos.y - size.y, uv.x + us.x, uv.y - us.y);
	CreateVertsXY(pos.x - hx, pos.y - size.y, uv.x, uv.y - us.y);
	CreateVertsXY(pos.x - hx, pos.y, uv.x, uv.y);
	CreateVertsXY(pos.x + hx, pos.y, uv.x + us.x, uv.y);

	CreateVertsYZ(pos.z - hx, pos.y - size.y, uv.x, uv.y - us.y);
	CreateVertsYZ(pos.z + hx, pos.y, uv.x + us.x, uv.y);
	CreateVertsYZ(pos.z + hx, pos.y - size.y, uv.x + us.x, uv.y - us.y);
	CreateVertsYZ(pos.z - hx, pos.y - size.y, uv.x, uv.y - us.y);
	CreateVertsYZ(pos.z - hx, pos.y, uv.x, uv.y);
	CreateVertsYZ(pos.z + hx, pos.y, uv.x + us.x, uv.y);

	vertSize = vc / stride;
	bbox.update(vec3f(pos.x, pos.y, pos.z));
	bbox.update(vec3f(pos.x + size.x, pos.y + size.y, pos.z + size.x));
}

#define updateCoordsXY(x,y)							\
		verts[p] = x; verts[p + 1] = y; p+=stride; 	\

void Pi3Cmesh::updateRectCoords2D(std::vector<float> &verts, uint32_t &p, const float x, const float y, const float w, const float h)
{
	updateCoordsXY(x, y - h);
	updateCoordsXY(x + w, y);
	updateCoordsXY(x + w, y - h);
	updateCoordsXY(x, y - h);
	updateCoordsXY(x, y);
	updateCoordsXY(x + w, y);
}

void Pi3Cmesh::updateRectCoordsROT(std::vector<float> &verts, uint32_t &p, const float x, const float y,  const float ax, const float ay) 
{
	updateCoordsXY(x - ax, y + ay);
	updateCoordsXY(x + ax, y - ay);
	updateCoordsXY(x - ay, y - ax);
	updateCoordsXY(x - ax, y + ay);
	updateCoordsXY(x + ay, y + ax);
	updateCoordsXY(x + ax, y - ay);
}

#define updateCoordsXYZ(x,y,z)									\
	verts[p] = x; verts[p+1] = y; verts[p+2] = z; p+=stride;	\


void Pi3Cmesh::updateRectCoordsBB(std::vector<float> &verts, uint32_t &p, const vec3f &pos, const float ax, const float az, const float sy) {
	updateCoordsXYZ(pos.x - ax, pos.y, pos.z - az); // , uv.x, uv.y);
	updateCoordsXYZ(pos.x + ax, pos.y + sy, pos.z + az); // , uv.x + us.x, uv.y);
	updateCoordsXYZ(pos.x + ax, pos.y, pos.z + az); // , uv.x + us.x, uv.y + us.y);
	updateCoordsXYZ(pos.x - ax, pos.y, pos.z - az); // , uv.x, uv.y);
	updateCoordsXYZ(pos.x - ax, pos.y + sy, pos.z - az); // , uv.x, uv.y + us.y);
	updateCoordsXYZ(pos.x + ax, pos.y + sy, pos.z + az); // , uv.x + us.x, uv.y + us.y);
}

void Pi3Cmesh::updateRectCoordsPSP(std::vector<float> &verts, uint32_t &p, const float x, const float y, const float z, const float cx, const float cy, const float ax, const float sy, const float w)
{
	updateCoordsXYZ(cx - (x - ax)*w, cy - y * w, z); // , uv.x, uv.y);
	updateCoordsXYZ(cx - (x + ax)*w, cy - (y + sy)*w, z); // , uv.x + us.x, uv.y);
	updateCoordsXYZ(cx - (x + ax)*w, cy - y * w, z); // , uv.x + us.x, uv.y + us.y);
	updateCoordsXYZ(cx - (x - ax)*w, cy - y * w, z); // , uv.x, uv.y);
	updateCoordsXYZ(cx - (x - ax)*w, cy - (y + sy)*w, z); // , uv.x, uv.y + us.y);
	updateCoordsXYZ(cx - (x + ax)*w, cy - (y + sy)*w, z); // , uv.x + us.x, uv.y + us.y);
}

float Pi3Cmesh::checkColliderGrid(const vec3f &p, const Pi3Cmatrix &mtx, float prevHeight)
{
	/* Check if a point lies within a triangle bounds using the colliderGrid ...*/
	float w = 9.999f / bbox.width();
	float d = 9.999f / bbox.depth();

	int xp = (int)((p.x - bbox.min.x) * w);
	int zp = (int)((p.z - bbox.min.z) * d);
	if (xp < 0 || xp>9 || zp < 0 || zp>9) return prevHeight;

	std::vector<float> &vp = xgrid[xp][zp];

	for (size_t v = 0; v < vp.size(); v += 9) {
		if (p.x > vp[v] && p.x > vp[v + 3] && p.x > vp[v + 6]) continue;
		if (p.x < vp[v] && p.x < vp[v + 3] && p.x < vp[v + 6]) continue;
		if (p.z > vp[v + 2] && p.z > vp[v + 5] && p.z > vp[v + 8]) continue;
		if (p.z < vp[v + 2] && p.z < vp[v + 5] && p.z < vp[v + 8]) continue;
		
		vec3f v1 = mtx.transformVec(vec3f(vp[v], vp[v + 1], vp[v + 2]));
		vec3f v2 = mtx.transformVec(vec3f(vp[v + 3], vp[v + 4], vp[v + 5]));
		vec3f v3 = mtx.transformVec(vec3f(vp[v + 6], vp[v + 7], vp[v + 8]));

		float d1 = Pi3Ccollision::rayIntersectFloor(v2 - v1, v3 - v1, p - v1);
		if (d1 > 0 && d1 < prevHeight) {
			return d1;
		}
	}
	return prevHeight; //none found
}

bool Pi3Cmesh::createColliderGrid()
{
	if (verts.size() < (5000*stride*3)) return false; //less than 1000 tris, then return

	//Consolidates vertices into a x/z grid so that ground collisions can be tested considerably faster
	float w = 9.999f / bbox.width();
	float d = 9.999f / bbox.depth();
	float mx = bbox.min.x;
	float mz = bbox.min.z;
	uint32_t stride2 = stride * 2;

	for (size_t v = 0; v < vc- stride * 3; v += stride * 3)
	{
		int xp = (int)((verts[v] - mx) * w);
		int zp = (int)((verts[v+2] - mz) * d);
		xgrid[xp][zp].push_back(verts[v]);
		xgrid[xp][zp].push_back(verts[v + 1]);
		xgrid[xp][zp].push_back(verts[v + 2]);
		xgrid[xp][zp].push_back(verts[v + stride]);
		xgrid[xp][zp].push_back(verts[v + stride + 1]);
		xgrid[xp][zp].push_back(verts[v + stride + 2]);
		xgrid[xp][zp].push_back(verts[v + stride2]);
		xgrid[xp][zp].push_back(verts[v + stride2 + 1]);
		xgrid[xp][zp].push_back(verts[v + stride2 + 2]);
		int xp2 = (int)((verts[v + stride] - mx) * w);
		int zp2 = (int)((verts[v + stride + 2] - mz) * d);
		if (xp2 != xp || zp2 != zp) {
			xgrid[xp2][zp2].push_back(verts[v]);
			xgrid[xp2][zp2].push_back(verts[v + 1]);
			xgrid[xp2][zp2].push_back(verts[v + 2]);
			xgrid[xp2][zp2].push_back(verts[v + stride]);
			xgrid[xp2][zp2].push_back(verts[v + stride + 1]);
			xgrid[xp2][zp2].push_back(verts[v + stride + 2]);
			xgrid[xp2][zp2].push_back(verts[v + stride2]);
			xgrid[xp2][zp2].push_back(verts[v + stride2 + 1]);
			xgrid[xp2][zp2].push_back(verts[v + stride2 + 2]);
		}
		int xp3 = (int)((verts[v + stride2] - mx) * w);
		int zp3 = (int)((verts[v + stride2 + 2] - mz) * d);
		if ((xp3 != xp || zp3 != zp) && (xp3 != xp2 || zp3 != zp2)) {
			xgrid[xp3][zp3].push_back(verts[v]);
			xgrid[xp3][zp3].push_back(verts[v + 1]);
			xgrid[xp3][zp3].push_back(verts[v + 2]);
			xgrid[xp3][zp3].push_back(verts[v + stride]);
			xgrid[xp3][zp3].push_back(verts[v + stride + 1]);
			xgrid[xp3][zp3].push_back(verts[v + stride + 2]);
			xgrid[xp3][zp3].push_back(verts[v + stride2]);
			xgrid[xp3][zp3].push_back(verts[v + stride2 + 1]);
			xgrid[xp3][zp3].push_back(verts[v + stride2 + 2]);
		}
	}
	return true;
}
