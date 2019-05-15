#include "Pi3Cmesh.h"
#include "Pi3Ccollision.h"
#include <algorithm>
#include <unordered_set>
#include <cmath>

Pi3Cmesh::Pi3Cmesh(const std::string &name, const uint32_t stride) : name(name), stride(stride) {
	reset();
}

void Pi3Cmesh::reset()
{
	mode = GL_TRIANGLES; 
	//name = ""; 
	vertOffset = 0; 
	vertSize = 0;
	verts.resize(0);
	bbox.init();
	materialRef = -1;
	//stride = defaultStride;
	vc = 0;
}
	
void Pi3Cmesh::addPackedVert(const vec3f &position, const vec3f &normal, const vec2f &uv, const GLfloat *cols)
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
	if (cols && stride==12) {
		verts[vc++] = cols[0];
		verts[vc++] = cols[1];
		verts[vc++] = cols[2];
		verts[vc++] = cols[3];
	}
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
	newmesh.dynamic = dynamic;
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

	for (size_t i = voff; i < (voff + vsize); i += stride * 3)
	{
		uint32_t i2 = i + stride;
		uint32_t i3 = i + stride + stride;

		// Calc Z's and discard any triangle that has a point behind the viewer ...
		float z1 = mtx.transformZ(&mverts[i]); if (z1 > 0) continue;
		float z2 = mtx.transformZ(&mverts[i2]); if (z2 > 0) continue;
		float z3 = mtx.transformZ(&mverts[i3]); if (z3 > 0) continue;

		float w1 = touch.perspective / z1, w2 = touch.perspective / z2, w3 = touch.perspective / z3;

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
		if ((-yw2 * xw3 + yw1 * (-xw2 + xw3) + xw1 * (yw2 - yw3) + xw2 * yw3) > 0) continue;

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
}

void Pi3Cmesh::createSharedTriangleList(const std::vector<float> &mverts, std::vector<uint32_t> &vertindexes, std::vector<float> &newverts, std::vector<uint32_t> &uvindexes, std::vector<float> &newuvs, const float tolerance)
{
	//From a soup of triangles, work out which triangles have matching vertices and count them as 'shared'

	std::unordered_set<std::mapvert> mapverts;

	float itol = 1.f / tolerance;
	uint32_t index = 0;
	for (size_t i = vertOffset; i < (vertOffset + vc); i += stride) {
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

	std::unordered_set<std::mapuv> mapuvs;

	index = 0;
	for (size_t i = 0; i < vc; i += stride) {
		std::mapuv v(mverts[i + 6], mverts[i + 7]);
		auto it = mapuvs.find(v);
		if (it == mapuvs.end()) {
			newuvs.push_back(mverts[i + 6]);
			newuvs.push_back(mverts[i + 7]);
			uvindexes.push_back(index);
			v.index = index++;
			mapuvs.insert(v);
		}
		else {
			uvindexes.push_back(it->index);
		}
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
	uint32_t sf = 3 * sizeof(float);

	for (size_t i = min; i < newmax; i += stride*3) {
		memcpy(&v0, &verts[i], sf);
		memcpy(&v1, &verts[i + x2], sf);
		memcpy(&v2, &verts[i + x3], sf);
		vec3f n = -((v0 - v1) ^ (v1 - v2)).unit();
		memcpy(&verts[i + 3], &n, sf);
		memcpy(&verts[i + x2 + 3], &n, sf);
		memcpy(&verts[i + x3 + 3], &n, sf);
	}
}

#define CreateVerts(x,y,ux,uy)									\
		verts[vc++] = x; verts[vc++] = y; verts[vc++] = pos.z;	\
		verts[vc++] = 0; verts[vc++] = -1.f; verts[vc++] = 0;	\
		verts[vc++] = ux; verts[vc++] = 0.9999f-uy;				\

void Pi3CspriteArray::addSprite(const vec3f &pos, const vec2f &size, const vec2f &uv, const vec2f &us)
{
	if (vc + stride * 6 > verts.size()) verts.resize(vc + 48);
	CreateVerts(pos.x, pos.y - size.y, uv.x, uv.y - us.y);
	CreateVerts(pos.x + size.x, pos.y , uv.x + us.x, uv.y);
	CreateVerts(pos.x + size.x, pos.y - size.y, uv.x + us.x, uv.y - us.y);
	CreateVerts(pos.x, pos.y - size.y, uv.x, uv.y - us.y);
	CreateVerts(pos.x, pos.y , uv.x, uv.y);
	CreateVerts(pos.x + size.x, pos.y, uv.x + us.x, uv.y);

	vertSize = vc / stride;
	bbox.update(vec3f(pos.x, pos.y, pos.z));
	bbox.update(vec3f(pos.x + size.x, pos.y + size.y, pos.z));
}

#define updateCoordsXY(x,y)							\
		verts[p] = x; verts[p + 1] = y; p+=stride; 	\

void Pi3CspriteArray::updateSpriteCoords(std::vector<float> &verts, const uint32_t spriteRef, const float x, const float y)
{
	uint32_t p = (vertOffset + spriteRef * 6) * stride;
	float w = verts[p + stride] - verts[p];					//derive width and height from rectangle
	float h = verts[p + stride + 1] - verts[p + 1];
	updateCoordsXY(x, y - h);
	updateCoordsXY(x + w, y);
	updateCoordsXY(x + w, y - h);
	updateCoordsXY(x, y - h);
	updateCoordsXY(x, y);
	updateCoordsXY(x + w, y);
}

#define updateRotateCoordsXY(a,b)								\
		verts[p] = pos.x + a; verts[p + 1] = pos.y + b; p+=stride; 	\

void Pi3CspriteArray::updateSpriteCoordsRotated(std::vector<float> &verts, const uint32_t spriteRef, const vec2f &pos, const vec2f &size, const float angle)
{
	uint32_t p = vertOffset * stride + spriteRef * stride * 6;
	float hw = size.x * 0.5f;
	float hh = size.y * 0.5f;
	//float cx = pos.x + hw;
	//float cy = pos.y + hh;

	float ca = cos(angle);
	float sa = sin(angle);
	float ax = hw* ca - hh* sa;
	float ay = hw * sa + hh* ca;


	updateRotateCoordsXY(-ax, ay);
	updateRotateCoordsXY(ax, -ay);
	updateRotateCoordsXY(-ay, -ax);
	updateRotateCoordsXY(-ax, ay);
	updateRotateCoordsXY(ay, ax);
	updateRotateCoordsXY(ax, -ay);
}


#define updateCoordsXYZ(x,y,z)							\
		verts[p] = x; verts[p+1] = y; verts[p+2] = z;	\
		p+=stride; 										\

/* Transforms a 2D rect into 3D space with the effect of bill-boarding */
void Pi3CspriteArray::updateSpriteTransformCoords(std::vector<float> &verts, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent) //, const vec2f &uv, const vec2f &us
{
	//Transform pos into screen space ...
	uint32_t p = vertOffset * stride + spriteRef * stride * 6;

	vec3f tpos = scene_matrix->transformVec(pos);
	float hw = size.x * 0.5f;
	//float hh = size.y * 0.5f;
	float cx = cent.x, cy = cent.y;
	float w = 800.f / tpos.z;
	updateCoordsXYZ(cx - (tpos.x - hw)*w, cy - tpos.y*w, tpos.z); // , uv.x, uv.y);
	updateCoordsXYZ(cx - (tpos.x + hw)*w, cy - (tpos.y + size.y)*w, tpos.z); // , uv.x + us.x, uv.y);
	updateCoordsXYZ(cx - (tpos.x + hw)*w, cy - tpos.y*w, tpos.z); // , uv.x + us.x, uv.y + us.y);
	updateCoordsXYZ(cx - (tpos.x - hw)*w, cy - tpos.y*w, tpos.z); // , uv.x, uv.y);
	updateCoordsXYZ(cx - (tpos.x - hw)*w, cy - (tpos.y + size.y)*w, tpos.z); // , uv.x, uv.y + us.y);
	updateCoordsXYZ(cx - (tpos.x + hw)*w, cy - (tpos.y + size.y)*w, tpos.z); // , uv.x + us.x, uv.y + us.y);
}

void Pi3CspriteArray::updateSpriteBillboard(std::vector<float> &verts, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const vec3f &lookat)
{
	//Transform pos into screen space ...
	uint32_t p = vertOffset * stride + spriteRef * stride * 6;

	float hw = size.x * 0.5f;
	//float hh = size.y * 0.5f;

	float lx = lookat.x - pos.x;
	float lz = lookat.z - pos.z;
	float d = hw / sqrtf(lx*lx + lz*lz);
	float ax = lz*d, az = -lx*d;

	updateCoordsXYZ(pos.x - ax, pos.y, pos.z - az); // , uv.x, uv.y);
	updateCoordsXYZ(pos.x + ax, (pos.y + size.y), pos.z + az); // , uv.x + us.x, uv.y);
	updateCoordsXYZ(pos.x + ax, pos.y, pos.z + az); // , uv.x + us.x, uv.y + us.y);
	updateCoordsXYZ(pos.x - ax, pos.y, pos.z - az); // , uv.x, uv.y);
	updateCoordsXYZ(pos.x - ax, (pos.y + size.y), pos.z - az); // , uv.x, uv.y + us.y);
	updateCoordsXYZ(pos.x + ax, (pos.y + size.y), pos.z + az); // , uv.x + us.x, uv.y + us.y);
}
