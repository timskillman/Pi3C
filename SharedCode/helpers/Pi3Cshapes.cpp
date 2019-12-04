#include "Pi3Cshapes.h"
#ifdef __WINDOWS__
#include <cmath>
#else
#include <math.h>
#endif

#include "Pi3Cutils.h"
//#include "tessellate.h"

#include <algorithm>

#define DEPTH2D -10.f

namespace Pi3Cshapes {

	float angleBetweenTwoVectors2D(vec2f v1, vec2f v2)
	{
		float prod = v1.x * v2.y - v1.y * v2.x;
		return fabs((prod < 0 ? -1.0f : 1.0f) * acosf((v1.x * v2.x + v1.y * v2.y) / (v1.length() * v2.length())));
	}

	vec2f dot(vec2f p1, vec2f p2)
	{
		//returns a normalized dot product ...
		float a = p2.x - p1.x;
		float b = p1.y - p2.y;
		float s = sqrt(a * a + b * b);
		if (s > 0.0f) return vec2f(b / s, a / s);
		return vec2f(0.0f, 0.0f);
	}

	void storeVNTC(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec3f& norm, const vec2f& texcoords, const uint32_t col)
	{
		/* Either creates new vertices or updates them */
		if (vc + 9 > verts.size()) verts.resize(vc + 9);
		verts[vc++] = pos.x;
		verts[vc++] = pos.y;
		verts[vc++] = pos.z;
		verts[vc++] = norm.x;
		verts[vc++] = norm.y;
		verts[vc++] = norm.z;
		verts[vc++] = texcoords.x;
		verts[vc++] = texcoords.y;
		verts[vc++] = (float)(col & 255) / 256.f + (float)((col >> 8) & 255) + (float)((col >> 16) & 255) * 256.f; //range 65536.255
	}

	void rect_verts(std::vector<float>& verts, uint32_t& vc, const vec2f& pos, const vec2f& size, const vec2f& uvpos, const vec2f& uvsize, const uint32_t col)
	{
		vec3f n(0, -1.f, 0);
		vec3f pos3(pos.x, pos.y, DEPTH2D);

		storeVNTC(verts, vc, pos3 + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col);
		storeVNTC(verts, vc, pos3 + vec3f(0, size.y, 0), n, vec2f(uvpos.x, uvpos.y + uvsize.y), col);
		storeVNTC(verts, vc, pos3 + vec3f(0, 0, 0), n, uvpos, col);

		storeVNTC(verts, vc, pos3 + vec3f(size.x, 0, 0), n, vec2f(uvpos.x + uvsize.x, uvpos.y), col);
		storeVNTC(verts, vc, pos3 + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col);
		storeVNTC(verts, vc, pos3 + vec3f(0, 0, 0), n, uvpos, col);
	}

	Pi3Cmesh rect(const vec2f& pos, const vec2f& size, const uint32_t col, const vec2f& uvpos, const vec2f& uvsize)
	{
		Pi3Cmesh rect("rect");
		rect.stride = 9;

		rect_verts(rect.verts, rect.vc, pos, size, uvpos, uvsize, col);

		rect.bbox.bboxFromVerts(rect.verts, 0, rect.vc, rect.stride);
		rect.materialRef = 0;
		return rect;
	}

	void quad_verts(std::vector<float>& verts, uint32_t& vc, const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec3f& p4, const vec2f& uvpos, const vec2f& uvsize, const uint32_t col)
	{
		vec3f n = p1.trinormal(p2, p3);
		storeVNTC(verts, vc, p3, n, uvpos + uvsize, col);
		storeVNTC(verts, vc, p2, n, uvpos, col);
		storeVNTC(verts, vc, p1, n, vec2f(uvpos.x, uvpos.y + uvsize.y), col);

		storeVNTC(verts, vc, p4, n, vec2f(uvpos.x + uvsize.x, uvpos.y), col);
		storeVNTC(verts, vc, p3, n, uvpos, col);
		storeVNTC(verts, vc, p1, n, uvpos + uvsize, col);
	}

	Pi3Cmesh quad(const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec3f& p4, const uint32_t col, const vec2f& uvpos, const vec2f& uvsize)
	{
		Pi3Cmesh quad("quad");
		quad.stride = 9;

		quad_verts(quad.verts, quad.vc, p1, p2, p3, p4, uvpos, uvsize, col);

		quad.bbox.bboxFromVerts(quad.verts, 0, quad.vc, quad.stride);
		quad.materialRef = 0;
		return quad;
	}

	void rectLine_verts(std::vector<float>& verts, uint32_t& vc, const vec2f& pos, const vec2f& size, const vec2f& uvpos, const vec2f& uvsize, const uint32_t col)
	{
		vec3f n(0, -1.f, 0);
		vec3f pos3(pos.x, pos.y, DEPTH2D);
		storeVNTC(verts, vc, pos3 + vec3f(0, 0, 0), n, uvpos, col);
		storeVNTC(verts, vc, pos3 + vec3f(0, size.y, 0), n, vec2f(uvpos.x, uvpos.y + uvsize.y), col);
		storeVNTC(verts, vc, pos3 + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col);
		storeVNTC(verts, vc, pos3 + vec3f(size.x, 0, 0), n, vec2f(uvpos.x + uvsize.x, uvpos.y), col);
		storeVNTC(verts, vc, pos3 + vec3f(0, 0, 0), n, uvpos, col);
	}

	void rectThickLine_verts(std::vector<float>& verts, uint32_t& vc, const vec2f& pos, const vec2f& size, const float thickness, const vec2f& uvpos, const vec2f& uvsize, const uint32_t col, bool triangles)
	{
		vec3f n(0, -1.f, 0);
		vec3f pos3(pos.x, pos.y, DEPTH2D);
		float uvx = thickness / size.x; float uvy = thickness / size.y;
		storeVNTC(verts, vc, pos3 + vec3f(thickness, thickness,0), n, uvpos + vec2f(uvx,uvy), col); //0
		storeVNTC(verts, vc, pos3, n, uvpos, col); //1
		storeVNTC(verts, vc, pos3 + vec3f(0, size.y, 0), n, vec2f(uvpos.x, uvpos.y + uvsize.y), col); //2
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(thickness, thickness, 0), n, uvpos + vec2f(uvx, uvy), col); //0
			storeVNTC(verts, vc, pos3 + vec3f(0, size.y, 0), n, vec2f(uvpos.x, uvpos.y + uvsize.y), col); //2
		}
		storeVNTC(verts, vc, pos3 + vec3f(thickness, size.y - thickness, 0), n, uvpos + vec2f(uvx, uvsize.y-uvy), col); //3
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(thickness, size.y - thickness, 0), n, uvpos + vec2f(uvx, uvsize.y - uvy), col); //3
			storeVNTC(verts, vc, pos3 + vec3f(0, size.y, 0), n, vec2f(uvpos.x, uvpos.y + uvsize.y), col); //2
		}
		storeVNTC(verts, vc, pos3 + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col); //4
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(thickness, size.y - thickness, 0), n, uvpos + vec2f(uvx, uvsize.y - uvy), col); //3
			storeVNTC(verts, vc, pos3 + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col); //4
		}
		storeVNTC(verts, vc, pos3 + vec3f(size.x-thickness, size.y-thickness, 0), n, uvpos + vec2f(uvsize.x - uvx, uvsize.y - uvy), col); //5
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(size.x - thickness, size.y - thickness, 0), n, uvpos + vec2f(uvsize.x - uvx, uvsize.y - uvy), col); //5
			storeVNTC(verts, vc, pos3 + vec3f(size.x, size.y, 0), n, uvpos + uvsize, col); //4
		}
		storeVNTC(verts, vc, pos3 + vec3f(size.x, 0, 0), n, vec2f(uvpos.x + uvsize.x, uvpos.y), col); //6
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(size.x - thickness, size.y - thickness, 0), n, uvpos + vec2f(uvsize.x - uvx, uvsize.y - uvy), col); //5
			storeVNTC(verts, vc, pos3 + vec3f(size.x, 0, 0), n, vec2f(uvpos.x + uvsize.x, uvpos.y), col); //6
		}
		storeVNTC(verts, vc, pos3 + vec3f(size.x - thickness, thickness, 0), n, vec2f(uvpos.x + uvsize.x - uvx, uvpos.y + uvy), col); //7
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(size.x - thickness, thickness, 0), n, vec2f(uvpos.x + uvsize.x - uvx, uvpos.y + uvy), col); //7
			storeVNTC(verts, vc, pos3 + vec3f(size.x, 0, 0), n, vec2f(uvpos.x + uvsize.x, uvpos.y), col); //6
		}
		storeVNTC(verts, vc, pos3, n, uvpos, col); //8
		if (triangles) {
			storeVNTC(verts, vc, pos3 + vec3f(size.x - thickness, thickness, 0), n, vec2f(uvpos.x + uvsize.x - uvx, uvpos.y + uvy), col); //7
			storeVNTC(verts, vc, pos3, n, uvpos, col); //8
		}
		storeVNTC(verts, vc, pos3 + vec3f(thickness, thickness, 0), n, uvpos + vec2f(uvx, uvy), col); //9
	}

	Pi3Cmesh rectLine(const vec2f& pos, const vec2f& size, const float thickness, const uint32_t col, const vec2f& uvpos, const vec2f& uvsize)
	{
		Pi3Cmesh rect("rectline");
		rect.stride = 9;

		if (thickness == 0) {
			rectLine_verts(rect.verts, rect.vc, pos, size, uvpos, uvsize, col);
		}
		else {
			rectThickLine_verts(rect.verts, rect.vc, pos, size, thickness, uvpos, uvsize, col, true); //GL_TRIANGLE_STRIP = false
		}

		rect.bbox.bboxFromVerts(rect.verts, 0, rect.vc, rect.stride);
		rect.materialRef = 0;
		return rect;
	}

	void plane_verts(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec2f& size, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f& uvsize, const uint32_t col)
	{
		vec3f vec;
		float v[3];
		int a = 0; int b = 1; int c = 2;
		vec3f n(0, 0, -1.f);
		float sizex = size.x;

		switch (direction) {
		case FACE_FRONT:	sizex = -sizex; break;
		case FACE_BACK:		n = vec3f(0, 0, 1.f); break;
		case FACE_RIGHT:	a = 2; b = 1; c = 0; n = vec3f(-1.f, 0, 0); break;
		case FACE_LEFT:		a = 2; b = 1; c = 0; n = vec3f(1.f, 0, 0); sizex = -sizex; break;
		case FACE_TOP:		a = 0; b = 2; c = 1; n = vec3f(0, 1.f, 0); break;
		case FACE_BOTTOM:	a = 0; b = 2; c = 1; n = vec3f(0, -1.f, 0); sizex = -sizex; break;
		}

		const float xs = 1.f / static_cast<float>(xdivs);
		const float ys = 1.f / static_cast<float>(ydivs);
		const float xd = xs * sizex;
		const float yd = ys * size.y;
		const float ux = xs * uvsize.x;
		const float uy = ys * uvsize.y;

		v[c] = 0;

		for (float yi = -0.5f; yi < 0.4999f; yi += ys) {
			for (float xi = -0.5f; xi < 0.4999f; xi += xs) {
				float x = xi * sizex; float y = yi * size.y;
				float xu = (xi + 0.5f) * uvsize.x; float yu = (yi + 0.5f) * uvsize.y;
				v[a] = x + xd; v[b] = y + yd; memcpy(&vec, &v, 12);
				storeVNTC(verts, vc, pos + vec, n, vec2f(xu + ux, yu + uy), col);
				v[a] = x; v[b] = y + yd; memcpy(&vec, &v, 12);
				storeVNTC(verts, vc, pos + vec, n, vec2f(xu, yu + uy), col);
				v[a] = x; v[b] = y; memcpy(&vec, &v, 12);
				storeVNTC(verts, vc, pos + vec, n, vec2f(xu, yu), col);

				v[a] = x + xd; v[b] = y; memcpy(&vec, &v, 12);
				storeVNTC(verts, vc, pos + vec, n, vec2f(xu + ux, yu), col);
				v[a] = x + xd; v[b] = y + yd; memcpy(&vec, &v, 12);
				storeVNTC(verts, vc, pos + vec, n, vec2f(xu + ux, yu + uy), col);
				v[a] = x; v[b] = y; memcpy(&vec, &v, 12);
				storeVNTC(verts, vc, pos + vec, n, vec2f(xu, yu), col);
			}
		}
	}

	Pi3Cmesh plane(const vec3f& pos, const vec2f& size, const uint32_t xdivs, const uint32_t ydivs, int direction, const uint32_t col, const vec2f& uvsize)
	{
		Pi3Cmesh plane("Plane");
		plane.stride = 9;

		plane_verts(plane.verts, plane.vc, pos, size, xdivs, ydivs, direction, uvsize, col);

		plane.bbox.bboxFromVerts(plane.verts, 0, plane.vc, plane.stride);
		plane.materialRef = 0;
		return plane;
	}

	const float cubeOffs[72] = { -1.f,1.f,-1.f, 1.f,1.f,-1.f, 1.f,-1.f,-1.f, -1.f,-1.f,-1.f,
		1.f,1.f,-1.f, 1.f,1.f,1.f, 1.f,-1.f,1.f, 1.f,-1.f,-1.f,
		1.f,1.f,1.f, -1.f,1.f,1.f, -1.f,-1.f,1.f, 1.f,-1.f,1.f,
		-1.f,1.f,1.f, -1.f,1.f,-1.f, -1.f,-1.f,-1.f, -1.f,-1.f,1.f,
		-1.f,-1.f,-1.f, 1.f,-1.f,-1.f, 1.f,-1.f,1.f, -1.f,-1.f,1.f,
		1.f,1.f,-1.f, -1.f,1.f,-1.f, -1.f,1.f,1.f, 1.f,1.f,1.f };

	const float cnorms[18] = { 0,0,-1.0f, 1.0f,0,0, 0,0,1.0f, -1.0f,0,0, 0,-1.0f,0, 0,1.0f,0 };

	void cube_verts(std::vector<float>& verts, uint32_t& vc, const vec3f& pos, const vec3f& size, const int divx, const int divy, const int divz, const uint32_t col)
	{
		vec3f hsize = size / 2.f;
		for (int i = 0; i < 6; i++)
		{
			int v = i * 12;
			vec3f n = vec3f(cnorms[i * 3], cnorms[i * 3 + 1], cnorms[i * 3 + 2]);
			storeVNTC(verts, vc, pos + vec3f(hsize.x * cubeOffs[v + 6], hsize.y * cubeOffs[v + 7], hsize.z * cubeOffs[v + 8]), n, vec2f(1, 1), col);
			storeVNTC(verts, vc, pos + vec3f(hsize.x * cubeOffs[v], hsize.y * cubeOffs[v + 1], hsize.z * cubeOffs[v + 2]), n, vec2f(0, 0), col);
			storeVNTC(verts, vc, pos + vec3f(hsize.x * cubeOffs[v + 3], hsize.y * cubeOffs[v + 4], hsize.z * cubeOffs[v + 5]), n, vec2f(0, 1), col);

			storeVNTC(verts, vc, pos + vec3f(hsize.x * cubeOffs[v + 9], hsize.y * cubeOffs[v + 10], hsize.z * cubeOffs[v + 11]), n, vec2f(1, 0), col);
			storeVNTC(verts, vc, pos + vec3f(hsize.x * cubeOffs[v], hsize.y * cubeOffs[v + 1], hsize.z * cubeOffs[v + 2]), n, vec2f(0, 0), col);
			storeVNTC(verts, vc, pos + vec3f(hsize.x * cubeOffs[v + 6], hsize.y * cubeOffs[v + 7], hsize.z * cubeOffs[v + 8]), n, vec2f(1, 1), col);
		}
	}

	Pi3Cmesh cuboid(const vec3f& pos, const vec3f& size, const uint32_t col, const int divx, const int divy, const int divz)
	{
		Pi3Cmesh cube("Cuboid");
		cube.stride = 9;

		cube_verts(cube.verts, cube.vc, pos, size, divx, divy, divz, col);

		cube.bbox.bboxFromVerts(cube.verts, 0, cube.vc, cube.stride);
		cube.materialRef = 0;
		return cube;
	}

	void calcPathNormals(std::vector<vec2f>& path, std::vector<vec2f>& normals, const float creaseAngle, const bool joined, const float inv)
	{
		//Splits a path by creasAngle threshold
		//If the angle between two lines is greater than the creaseAngle, then split them.

		uint32_t sz = path.size();
		if (sz < 2) return;

		std::vector<vec2f> newPath;
		vec2f p1;
		vec2f p2 = path[0];
		vec2f p3 = path[1];
		vec2f norm;

		//work out first point in path
		newPath.push_back(p2);
		if (joined && sz > 2) {
			p1 = path[sz - 2];
			if (angleBetweenTwoVectors2D(p2 - p1, p3 - p2) > 0.5f) {
				normals.push_back(dot(p2, p3) * inv);
			}
			else {
				normals.push_back(dot(p1 - p2, p3 - p2) * inv);
			}
			p1 = path[0];
		}
		else
		{
			p1 = path[0];
			normals.push_back(dot(p2, p3) * inv);
		}

		//work out normals and points between end and last points
		if (sz > 2) {
			for (size_t i = 1; i < sz - 1; i++) {
				p2 = path[i]; p3 = path[i + 1];
				newPath.push_back(p2);
				if (angleBetweenTwoVectors2D(p2 - p1, p3 - p2) > 0.5f) {
					normals.push_back(dot(p1, p2));
					newPath.push_back(p2);
					normals.push_back(dot(p2, p3));
				}
				else {
					normals.push_back(dot(p1 - p2, p3 - p2));
				}
				p1 = p2;
			}

			//work out last point in path
			p2 = path[sz - 1];
			newPath.push_back(p2);

			if (joined) {
				p3 = path[1];
				if (angleBetweenTwoVectors2D(p2 - p1, p3 - p2) > 0.5f) {
					normals.push_back(dot(p1, p2));
				}
				else {
					normals.push_back(dot(p1 - p2, p3 - p2));
				}
			}
			else
			{
				normals.push_back(normals[normals.size() - 1]);
			}
		}
		else
		{
			normals.push_back(normals[0]);
			newPath.push_back(p3);
		}

		path = newPath;
	}

	void copyVerts(std::vector<float>& srcverts, std::vector<float>& destverts, const uint32_t stride, uint32_t& vc, uint32_t ref)
	{
		if (vc + stride > destverts.size()) destverts.resize(vc + stride);
		size_t p = ref * stride;
		memcpy(&destverts[vc], &srcverts[p], stride * sizeof(float));
		vc += stride;
	}

	void disk_verts(std::vector<float>& verts, uint32_t& vc, const vec3f &pos, const float innerRad, const float outerRad, const int sides, const uint32_t col)
	{
		std::vector<float> vertTemp;
		uint32_t tc = 0;
		uint32_t stride = 9;

		float rs = 2 * PI / (float)sides;
		float ux = 10.f / sides;

		for (int r = 0; r < sides; r++) {
			float a = rs * (float)r;
			storeVNTC(vertTemp, tc, pos + vec3f(cos(a) * innerRad, 0, sin(a) * innerRad), vec3f(0, 1.f, 0), vec2f(float(r) * ux, 0.999f), col);
		}

		for (int r = 0; r < sides; r++) {
			float a = rs * (float)r;
			storeVNTC(vertTemp, tc, pos + vec3f(cos(a) * outerRad, 0, sin(a) * outerRad), vec3f(0, 1.f, 0), vec2f(float(r) * ux, 0.f), col);
		}

		for (int p = 0; p < sides; p++) {
			//GL_TRIANGLES
			copyVerts(vertTemp, verts, stride, vc, sides + p);
			copyVerts(vertTemp, verts, stride, vc, (p + 1) % sides);
			copyVerts(vertTemp, verts, stride, vc, p);

			copyVerts(vertTemp, verts, stride, vc, sides + p);
			copyVerts(vertTemp, verts, stride, vc, sides + (p + 1) % sides);
			copyVerts(vertTemp, verts, stride, vc, (p + 1) % sides);
		}

	}

	Pi3Cmesh disk(const vec3f &pos, const float innerRad, const float outerRad, const uint32_t col, const int sides)
	{
		Pi3Cmesh model("disk");
		model.stride = 9;

		disk_verts(model.verts, model.vc, pos, innerRad, outerRad, sides, col);

		model.bbox.bboxFromVerts(model.verts, 0, model.vc, model.stride);
		model.mode = GL_TRIANGLES;
		return model;

	}

	void lathe_verts(std::vector<float>& verts, uint32_t& vc, const uint32_t stride, const vec3f& pos, std::vector<vec2f>& path, const int edges, const float rise, const float startAngle, const float endAngle, const bool invert, const float creaseAngle, const vec2f prevPoint, const vec2f lastPoint, const uint32_t uvtype, const uint32_t col)
	{
		float inv = (invert) ? -1.0f : 1.0f;
		std::vector<vec2f> normals;
		calcPathNormals(path, normals, 60, true, inv);
		uint32_t sz = path.size();

		float angDiff = endAngle - startAngle;
		float angStep = angDiff / (float)edges;

		float tcy = 0.0f;
		float tcx = 1.0f / (float)edges;
		float rdiv = rise / (float)edges;

		//Find min y / max y of path
		float miny = path[0].y; float maxy = miny;
		for (size_t p = 0; p < sz; p++) {
			if (path[p].y < miny) miny = path[p].y;
			if (path[p].y > maxy) maxy = path[p].y;
		}
		float cy = (maxy + miny) / 2.f;

		std::vector<float> vertTemp;
		uint32_t tc = 0;

		for (size_t p = 0; p < sz; p++) {

			//Apply UV mapping ...
			switch (uvtype) {
			case UVMAP_CYLINDER: //Cylinder UV mapping
				tcy = 1.0f - ((path[p].y - miny) / (maxy - miny));
				break;
			case UVMAP_SPHERE: //Spherical UV mapping
				tcy = atan2f(path[p].x, path[p].y - cy) / PI;
				if (tcy < 0.0f) tcy += 1.f;
				break;
			}

			float risey = path[p].y;
			for (float r = 0; r < edges; r++) {
				float ang = startAngle + (float)r * angStep;
				float sinr = sin(ang);
				float cosr = cos(ang);
				storeVNTC(vertTemp, tc, pos + vec3f(path[p].x * sinr, risey, path[p].x * cosr), vec3f(sinr * normals[p].x, normals[p].y, cosr * normals[p].x), vec2f(tcx * r, tcy), col);
				risey += rdiv;
			}

			storeVNTC(vertTemp, tc, pos + vec3f(0, path[p].y, path[p].x), vec3f(0, normals[p].y, normals[p].x), vec2f(0.9999f, tcy), col);
		}

		int pn = 0;
		int pp = 0;
		for (size_t p = 0; p < (sz - 1); p++) {
			pn += (edges + 1);
			if ((path[p + 1] != path[p]) || p == (sz - 2)) {
				for (int32_t r = 0; r < edges; r++)
				{
					copyVerts(vertTemp, verts, stride, vc, pn + r);
					copyVerts(vertTemp, verts, stride, vc, pp + r + 1);
					copyVerts(vertTemp, verts, stride, vc, pp + r);

					copyVerts(vertTemp, verts, stride, vc, pn + r);
					copyVerts(vertTemp, verts, stride, vc, pn + r + 1);
					copyVerts(vertTemp, verts, stride, vc, pp + r + 1);
				}
			}
			pp += (edges + 1);
		}
	}

	Pi3Cmesh lathe(const std::string &name, const vec3f &pos, std::vector<vec2f> &path, const uint32_t col, const int edges, const float rise, const float startAngle, const float endAngle, const bool invert, const float creaseAngle, const vec2f &prevPoint, const vec2f &lastPoint, const uint32_t uvtype)
	{
		Pi3Cmesh lathe(name);
		lathe.stride = 9;

		size_t sz = path.size();
		if (sz < 2) return lathe;

		lathe_verts(lathe.verts, lathe.vc, lathe.stride, pos, path, edges, rise, startAngle, endAngle, invert, creaseAngle, prevPoint, lastPoint, uvtype, col);

		lathe.bbox.bboxFromVerts(lathe.verts, 0, lathe.vc, lathe.stride);
		lathe.mode = GL_TRIANGLES;
		return lathe;
	}

	Pi3Cmesh sphere(const vec3f &pos, const float radius, const uint32_t col, const float hemi, const int slices, const int sides, const bool invert)
	{
		std::vector<vec2f> path;
		float st = (PI*(1.0f - hemi)) / static_cast<float>(slices);
		float ho = PI - (PI*(1.f - hemi));
		float inv = invert ? -1.0f : 1.0f;
		for (int r = 0; r <= slices; r++) {
			path.push_back(vec2f(radius*sin(static_cast<float>(r)*st+ho)*inv, radius*cos(static_cast<float>(r)*st+ho)));
		}
		return lathe("Sphere", pos, path, col, sides, 0.0f, 0.0f, 2.f*PI, false, 60.f, vec2f(1, path[0].y), vec2f(-1, path[path.size() - 1].y),UVMAP_SPHERE);
	}

	Pi3Cmesh torus(const vec3f &pos, const float radius, const float ringradius, const uint32_t col, const int ringrots, const int sides)
	{
		std::vector<vec2f> path;
		float st = PI2 / (float)ringrots;
		for (int r = 0; r < (ringrots + 1); r++) {
			path.push_back(vec2f(radius + ringradius*sin(r*st), ringradius*cos(r*st)));
		}
		return lathe("Torus", pos, path, col, sides, 0.0f, 0.0f, PI2, false, 60.f);
	}

	Pi3Cmesh spring(const vec3f &pos, const float radius, const float length, const int coils, const float coilradius, const int ringrots, const uint32_t col, const int sides)
	{
		std::vector<vec2f> path;
		//float rise = length / coils;
		float st = (PI * 2.f) / (float)ringrots;
		for (int r = 0; r < (ringrots + 1); r++) {
			path.push_back(vec2f(radius + coilradius*sin(r*st), coilradius*cos(r*st)));
		}
		return lathe("Spring",pos, path, col, sides, length, 0.0f, 2.f*PI*(float)coils);
	}

	Pi3Cmesh cone(const vec3f &pos, const float radius, const float height, const uint32_t col, const int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(0, height));
		path.push_back(vec2f(radius, 0));
		path.push_back(vec2f(0, 0));
		return lathe("Cone",pos, path, col, sides, 0.0f, 0.0f, 2.f*PI, true, 60.0f, vec2f(-radius, height*2), vec2f(-radius, 0));
	}

	Pi3Cmesh tcone(const vec3f &pos, const float botradius, const float topradius, const float height, const uint32_t col, const int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(0, height));
		path.push_back(vec2f(topradius, height));
		path.push_back(vec2f(botradius, 0));
		path.push_back(vec2f(0, 0));
		return lathe("Truncated Cone", pos, path, col, sides, 0.f, 0.0f, 2.f*PI, true, 60.0f, vec2f(-1.0f,height));
	}

	Pi3Cmesh cylinder(const vec3f &pos, const float radius, const float height, const uint32_t col, const int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(0, height));
		path.push_back(vec2f(radius, height));
		path.push_back(vec2f(radius, 0));
		path.push_back(vec2f(0, 0));
		return lathe("Cylinder", pos, path, col, sides, 0.0f, 0.0f, 2.f*PI, true, 60.0f, vec2f(-1.0f, height));
	}

	Pi3Cmesh tube(const vec3f &pos, const float radius1, const float radius2, const float height, const uint32_t col, const int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(radius1, height));
		path.push_back(vec2f(radius2, height));
		path.push_back(vec2f(radius2, 0));
		path.push_back(vec2f(radius1, 0));
		path.push_back(vec2f(radius1, height));
		return lathe( "Tube", pos, path, col, sides, 0.0f, 0.0f, 2.f*PI, true, 60.0f, vec2f(0, 0), vec2f(0, 0), UVMAP_SPHERE);
	}
	
	void texMap(std::vector<float> &vertBuffer, uint32_t vc, uint32_t stride, uint32_t uvoffset, uint8_t x, uint8_t y)
	{
		//Get X,Y bounds of vertices ...
		float minx = 1e20f; float maxx = -1e20f;
		float miny = 1e20f;float maxy = -1e20f;		
		for (size_t i = 0; i < vc; i += stride) {
			if (vertBuffer[i+x] < minx) minx = vertBuffer[i+x];
			if (vertBuffer[i+x] > maxx) maxx = vertBuffer[i+x];
			if (vertBuffer[i+y] < miny) miny = vertBuffer[i+y];
			if (vertBuffer[i+y] > maxy) maxy = vertBuffer[i+y];
		}

		float left = minx;
		float top = miny;
		float w = 1.f / (maxx-minx);
		float h = 1.f / (maxy-miny);

		for (size_t i = 0; i < vc; i += stride) {
			vertBuffer[i + uvoffset] = (vertBuffer[i+x] - left)*w;
			vertBuffer[i + uvoffset + 1] = (vertBuffer[i+y] - top)*h;
		}
	}

	vec3f orient(const float x, const float y, const float z, uint8_t orientation)
	{
		switch (orientation) {
		case FACE_FRONT: return vec3f(x, y, z); break;
		case FACE_TOP: return vec3f(x, z, y); break;
		case FACE_LEFT: return vec3f(z, y, x); break;
		}
		return vec3f(x, y, z);
	}

	void extrude_verts(std::vector<float> &verts, uint32_t &vc, const vec3f &pos, std::vector<std::vector<float>>& contours, const float depth, const uint16_t divs, const uint32_t col) 
	{

		//convert vec2f path to vector float path ...
		//std::vector<std::vector<float>> contours;
		//for (size_t i = 0; i < path.size(); i++) {
		//	contours.emplace_back();
		//	std::vector<vec2f>& contourxy = path[i];
		//	std::vector<float>& contour = contours.back();
		//	contour.resize(path.size() * 2);
		//	memcpy(&contour[0], &contourxy[0], contourxy.size() * 2 * sizeof(float));
		//}
		
		std::vector<float> tris;
		std::vector<std::vector<float>> edges;
		int triCount = Pi3Cutils::tesselatePath(contours, tris, edges);

/*		
		std::vector<std::vector<float>> contours;
		for (size_t i = 0; i < contoursxy.size(); i++) {
			contours.emplace_back();
			std::vector<vec2f>& contourxy = contoursxy[i];
			std::vector<float>& contour = contours.back();
			contour.resize(contourxy.size() * 2);
			memcpy(&contour[0], &contourxy[0], contourxy.size() * 2 * sizeof(float));
		}

		//Find leftmost polygon (always an outside polygon)
		float leftx = 1e20f;
		int f = -1;
		for (size_t c = 0; c < contours.size(); c++) {
			std::vector<float>& contour = contours[c];
			for (size_t p = 0; p < contour.size(); p += 2) {
				if (contour[p] < leftx) {
					leftx = contour[p];
					f = c;
				}
			}
		}

		//Work out CW/CCW of leftmost polygon ...
		std::vector<float>& contour = contours[f];
		float a = 0.f; size_t p = contour.size() - 2;
		for (size_t q = 0; q < contour.size() - 1; q += 2) {
			a += contour[p] * contour[q + 1] - contour[q] * contour[p + 1];
			p = q;
		}

		//reverse contours if inverted
		if (a > 0)
		{
			for (size_t a = 0; a < contours.size(); a++) {
				std::vector<float>& contour = contours[a];
				uint32_t vs = contour.size();
				for (size_t j = 0; j < (vs - 1) / 2; j += 2) {
					std::swap(contour[j], contour[vs - j - 2]);
					std::swap(contour[j + 1], contour[vs - j - 1]);
				}
			}
		}

		std::vector<float> tris;
		std::vector<std::vector<float>> edges;
		tessellate(&contours, NULL, &edges, true);	//Get outer edges
		tessellate(&contours, &tris, NULL, false);	//Get triangles
		int triCount = tris.size() / 9;
*/

		std::vector<float> tempVerts;
		uint32_t tc = 0;

		vec3f n = vec3f(0.f, 1.f, 0.f);
		for (int i = 0; i < triCount; i++) {  //Top faces
			int t = i * 9;
			storeVNTC(verts, vc, pos + vec3f(tris[t], depth, tris[t + 1]), n, vec2f(0, 0), col);
			storeVNTC(verts, vc, pos + vec3f(tris[t + 3], depth, tris[t + 4]), n, vec2f(0, 0), col);
			storeVNTC(verts, vc, pos + vec3f(tris[t + 6], depth, tris[t + 7]), n, vec2f(0, 0), col);
		}
		n = vec3f(0.f, -1.f, 0.f);
		for (int i = 0; i < triCount; i++) {  //Bottom faces
			int t = i * 9;
			storeVNTC(verts, vc, pos + vec3f(tris[t], 0, tris[t + 1]), n, vec2f(0, 0), col);
			storeVNTC(verts, vc, pos + vec3f(tris[t + 6], 0, tris[t + 7]), n, vec2f(0, 0), col);
			storeVNTC(verts, vc, pos + vec3f(tris[t + 3], 0, tris[t + 4]), n, vec2f(0, 0), col);
		}

		for (size_t i = 0; i < edges.size(); i++) {
			std::vector<float>& contour = edges[i];
			for (size_t j = 0; j < contour.size(); j += 2) {
				size_t k = (j + 2) % contour.size();
				vec2f norm = dot(vec2f(contour[j], contour[j + 1]), vec2f(contour[k], contour[k + 1]));
				storeVNTC(verts, vc, pos + vec3f(contour[j], 0, contour[j + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), col);
				storeVNTC(verts, vc, pos + vec3f(contour[k], 0, contour[k + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), col);
				storeVNTC(verts, vc, pos + vec3f(contour[k], depth, contour[k + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), col);

				storeVNTC(verts, vc, pos + vec3f(contour[k], depth, contour[k + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), col);
				storeVNTC(verts, vc, pos + vec3f(contour[j], depth, contour[j + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), col);
				storeVNTC(verts, vc, pos + vec3f(contour[j], 0, contour[j + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), col);
			}
		}

	}

	Pi3Cmesh extrude(const std::string &name, const vec3f &pos, std::vector<std::vector<float>> &contours, const float depth, const uint32_t col, const uint16_t divs)
	{
		Pi3Cmesh extrusion(name);
		extrusion.stride = 9;

		extrude_verts(extrusion.verts, extrusion.vc, pos, contours, depth, divs, col);

		extrusion.bbox.bboxFromVerts(extrusion.verts, 0, extrusion.vc, extrusion.stride);
		extrusion.mode = GL_TRIANGLES;

		texMap(extrusion.verts, extrusion.vc, extrusion.stride, 6,0,2);
		extrusion.materialRef = 0;
		return extrusion;
	}

#define twotris true

	void elevationMap_verts(std::vector<float>& verts, uint32_t& vc, Pi3Ctexture &tex, const vec3f &pos, const vec2f &size, float mapHeight, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f &uvsize, const uint32_t col)
	{

		uint32_t pitch = tex.GetPitch();
		uint8_t* map = tex.GetTexels();
		uint32_t bpp = tex.GetDepth();
		uint32_t h = tex.GetHeight();
		uint32_t w = tex.GetWidth();

		float wf = (float)w;
		float hf = (float)h;

		vec3f vec1, vec2, vec3, vec4, vec5;
		float v1[3], v2[3], v3[3], v4[3], v5[3];
		int a = 0; int b = 1; int c = 2;
		vec3f n(0, 0, -1.f);
		vec2f sz = size;

		switch (direction) {
		case 0: sz.x = -sz.x; break;
		case 1: n = vec3f(0, 0, 1.f); break;
		case 2: a = 2; b = 1; c = 0; n = vec3f(-1.f, 0, 0); break;
		case 3: a = 2; b = 1; c = 0; n = vec3f(1.f, 0, 0); sz.x = -sz.x; break;
		case 4: a = 0; b = 2; c = 1; n = vec3f(0, -1.f, 0); break;
		case 5: a = 0; b = 2; c = 1; n = vec3f(0, 1.f, 0); sz.x = -sz.x; break;
		}

		const float xs = 1.f / static_cast<float>(xdivs);
		const float ys = 1.f / static_cast<float>(ydivs);
		const float xd = xs * sz.x;
		const float yd = ys * sz.y;
		const float ux = xs * uvsize.x;
		const float uy = ys * uvsize.y;

		float mh = mapHeight / 255.f;

		float xc = (wf-1.f) / (float)xdivs;
		float yc = (hf-1.f) / (float)ydivs;
		
		float yy = 0;
		for (float yi = -0.5f; yi < 0.499f; yi += ys) {
			float xx = 0;
			for (float xi = -0.5f; xi < 0.499f; xi += xs) {

				float x = xi * sz.x; 
				float y = yi * sz.y;
				float xu = xi * uvsize.x; 
				float yu = yi * uvsize.y;

				float h00 = ((float)((map[((uint32_t)xx) * bpp + ((uint32_t)yy) * pitch]) & 255)) * mh;
				float h10 = ((float)((map[((uint32_t)(xx + xc)) * bpp + ((uint32_t)yy) * pitch]) & 255)) * mh;
				float h01 = ((float)((map[((uint32_t)xx) * bpp + ((uint32_t)(yy + yc)) * pitch]) & 255)) * mh;
				float h11 = ((float)((map[((uint32_t)(xx + xc)) * bpp + ((uint32_t)(yy + yc)) * pitch]) & 255)) * mh;
				
#if twotris
				v1[a] = x + xd; v1[b] = h11; v1[c] = y + yd;
				memcpy(&vec1, &v1, sizeof(v1));
				v2[a] = x; v2[b] = h00; v2[c] = y;
				memcpy(&vec2, &v2, sizeof(v2));
				v3[a] = x; v3[b] = h01; v3[c] = y + yd;
				memcpy(&vec3, &v3, sizeof(v3));
				v4[a] = x + xd; v4[b] = h10; v4[c] = y;
				memcpy(&vec4, &v4, sizeof(v4));

				n = vec1.trinormal(vec2, vec3);
				storeVNTC(verts, vc, pos + vec1, n, vec2f(xu + ux, yu + uy), col);
				storeVNTC(verts, vc, pos + vec3, n, vec2f(xu, yu + uy), col);
				storeVNTC(verts, vc, pos + vec2, n, vec2f(xu, yu), col);

				n = vec1.trinormal(vec4, vec2);
				storeVNTC(verts, vc, pos + vec1, n, vec2f(xu + ux, yu + uy), col);
				storeVNTC(verts, vc, pos + vec2, n, vec2f(xu, yu), col);
				storeVNTC(verts, vc, pos + vec4, n, vec2f(xu + ux, yu), col);

#else
				float hh = (h00 + h01 + h10 + h11) / 4.0f; //avg height for centre point

				v1[a] = x + xd; v1[b] = h11; v1[c] = y + yd;
				memcpy(&vec1, &v1, sizeof(v1));
				v2[a] = x; v2[b] = h00; v2[c] = y;
				memcpy(&vec2, &v2, sizeof(v2));
				v3[a] = x; v3[b] = h01; v3[c] = y + yd;
				memcpy(&vec3, &v3, sizeof(v3));
				v4[a] = x + xd; v4[b] = h10; v4[c] = y;
				memcpy(&vec4, &v4, sizeof(v4));
				v5[a] = x + xd * 0.5f; v5[b] = hh; v5[c] = y + yd * 0.5f; //centre point
				memcpy(&vec5, &v5, sizeof(v5));

				n = vec5.trinormal(vec2, vec3);
				storeVNTC(verts, vc, pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), col);
				storeVNTC(verts, vc, pos + vec2, n, vec2f(xu, yu), col);
				storeVNTC(verts, vc, pos + vec3, n, vec2f(xu, yu + uy), col);

				n = vec5.trinormal(vec3, vec1);
				storeVNTC(verts, vc, pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), col);
				storeVNTC(verts, vc, pos + vec3, n, vec2f(xu, yu + uy), col);
				storeVNTC(verts, vc, pos + vec1, n, vec2f(xu + ux, yu + uy), col);

				n = vec5.trinormal(vec1, vec4);
				storeVNTC(verts, vc, pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), col);
				storeVNTC(verts, vc, pos + vec1, n, vec2f(xu + ux, yu + uy), col);
				storeVNTC(verts, vc, pos + vec4, n, vec2f(xu + ux, yu), col);

				n = vec5.trinormal(vec4, vec2);
				storeVNTC(verts, vc, pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), col);
				storeVNTC(verts, vc, pos + vec4, n, vec2f(xu + ux, yu), col);
				storeVNTC(verts, vc, pos + vec2, n, vec2f(xu, yu), col);
#endif

				xx += xc;
			}
			yy += yc;
		}

	}

	Pi3Cmesh elevationMap(Pi3Ctexture &tex, const vec3f &pos, const vec2f &size, const float mapHeight, const uint32_t xdivs, const uint32_t ydivs, const int direction, const uint32_t col, const vec2f &uvsize)
	{
		Pi3Cmesh meshmap("Map");
		meshmap.stride = 9;

		meshmap.verts.reserve((xdivs + 1)*(ydivs + 1) * 4 * 3 * 9);
		elevationMap_verts(meshmap.verts, meshmap.vc, tex, pos, size, mapHeight, xdivs, ydivs, direction, uvsize, col);

		meshmap.bbox.bboxFromVerts(meshmap.verts, 0, meshmap.vc, meshmap.stride);
		meshmap.materialRef = 0;
		//meshmap.dynamic = true;
		return meshmap;
	}

	Pi3Cmesh createGeoSphere(int stacks = 32, int slices = 32) {
		Pi3Cmesh mesh;
		uint32_t col = 0xffffffff;

		//add vertices
		mesh.addPackedVert(vec3f(0, 1, 0), vec3f(0,1,0),vec2f(0,0.5f),col);

		for (int i = 1; i < stacks; i++) {
			double phi = PI * double(i) / stacks;
			double cosPhi = cos(phi);
			double sinPhi = sin(phi);
			for (int j = 0; j < slices; j++) {
				double theta = PI*2 * double(j) / slices;
				mesh.addPackedVert(vec3f((float)(cos(theta)*sinPhi), (float)cosPhi, (float)(sin(theta)*sinPhi)), vec3f(0, 1.f, 0), vec2f(0, 0.5f), col);
			}
		}
		mesh.addPackedVert(vec3f(0, -1.f, 0), vec3f(0, -1.f, 0), vec2f(1.f, 0.5f), col);

		////top row triangle fan 
		//for (int j = 1; j < slices; j++) {
		//	mesh.
		//	mesh.addTriangle(0, j, j + 1);
		//}
		//mesh.addTriangle(0, slices, 1);

		////triangle strips
		//for (int i = 0; i < stacks - 2; i++) {
		//	int top = i * slices + 1;
		//	int bottom = (i + 1)*slices + 1;

		//	for (int j = 0; j < slices - 1; j++) {
		//		mesh.addTriangle(bottom + j, bottom + j + 1, top + j + 1);
		//		mesh.addTriangle(bottom + j, top + j + 1, top + j);
		//	}

		//	mesh.addTriangle(bottom + slices - 1, bottom, top);
		//	mesh.addTriangle(bottom + slices - 1, top, top + slices - 1);
		//}

		////bottom row triangle fan
		//int last = mesh.getNumVertices() - 1;
		//for (int j = last - 1; j > last - slices; j--) {
		//	mesh.addTriangle(last, j, j - 1);
		//}
		//mesh.addTriangle(last, last - slices, last - 1);

		return mesh;
	}

	void grid_verts(std::vector<float>& verts, uint32_t& vc, const vec2f &size, const float majorStep, const float minorStep, const uint32_t mincol, const uint32_t majcol)
	{
		float syh = size.y / 2.f;
		float sxh = size.x / 2.f;

		float sw = -sxh;
		for (float y = -syh; y <= syh; y += minorStep) {
			storeVNTC(verts, vc, vec3f(sw, 0, y), vec3f(0, 1, 0), vec2f(0, 0), mincol);
			sw = -sw;
			storeVNTC(verts, vc, vec3f(sw, 0, y), vec3f(0, 1, 0), vec2f(0, 0), mincol);
		}
		storeVNTC(verts, vc, vec3f(-sw, 0, syh), vec3f(0, 1, 0), vec2f(0, 0), mincol);

		sw = -syh;
		for (float x = -sxh; x <= sxh; x += minorStep) {
			storeVNTC(verts, vc, vec3f(x, 0, sw), vec3f(0, 1, 0), vec2f(0, 0), mincol);
			sw = -sw;
			storeVNTC(verts, vc, vec3f(x, 0, sw), vec3f(0, 1, 0), vec2f(0, 0), mincol);
		}
		storeVNTC(verts, vc, vec3f(sxh, 0, -sw), vec3f(0, 1, 0), vec2f(0, 0), mincol);

		sw = -sxh;
		for (float y = -syh; y <= syh; y += majorStep) {
			storeVNTC(verts, vc, vec3f(sw, 0, y), vec3f(0, 1, 0), vec2f(0, 0), majcol);
			sw = -sw;
			storeVNTC(verts, vc, vec3f(sw, 0, y), vec3f(0, 1, 0), vec2f(0, 0), majcol);
		}
		storeVNTC(verts, vc, vec3f(-sw, 0, syh), vec3f(0, 1, 0), vec2f(0, 0), majcol);

		sw = -syh;
		for (float x = -sxh; x <= sxh; x += majorStep) {
			storeVNTC(verts, vc, vec3f(x, 0, sw), vec3f(0, 1, 0), vec2f(0, 0), majcol);
			sw = -sw;
			storeVNTC(verts, vc, vec3f(x, 0, sw), vec3f(0, 1, 0), vec2f(0, 0), majcol);
		}
		storeVNTC(verts, vc, vec3f(sxh, 0, -sw), vec3f(0, 1, 0), vec2f(0, 0), majcol);
	}

	Pi3Cmesh grid(const vec2f &size, const float majorStep, const float minorStep, const uint32_t mincol, const uint32_t majcol)
	{
		Pi3Cmesh mesh;
		mesh.stride = 9;

		grid_verts(mesh.verts, mesh.vc, size, majorStep, minorStep, mincol, majcol);

		mesh.bbox.bboxFromVerts(mesh.verts, 0, mesh.vc, mesh.stride);
		mesh.materialRef = 0;

		return mesh;

	}
}