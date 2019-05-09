#include "Pi3Cshapes.h"
#ifdef __WINDOWS__
#include <cmath>
#else
#include <math.h>
#endif

#include "tessellate.h"
#include <algorithm>

#define DEPTH2D -10.f

namespace Pi3Cshapes {


	float length2D(vec2f v) { return sqrt(v.x*v.x + v.y*v.y); }

	float angleBetweenTwoVectors2D(vec2f v1, vec2f v2)
	{
		double prod = v1.x*v2.y - v1.y*v2.x;
		return fabs((prod < 0 ? -1.0f : 1.0f)*acosf((v1.x*v2.x + v1.y*v2.y) / (length2D(v1)*length2D(v2))));
	}

	vec2f dot(vec2f p1, vec2f p2)
	{
		//returns a normalized dot product ...
		float a = p2.x - p1.x;
		float b = p1.y - p2.y;
		float s = sqrt(a*a + b*b);
		if (s > 0.0f) return vec2f(b / s, a / s);
		return vec2f(0.0f, 0.0f);
	}

	Pi3Cmesh rect(const vec2f &pos, const vec2f &size, const vec2f &uvpos, const vec2f &uvsize)
	{
		Pi3Cmesh rect("rect");
		float *cols = nullptr;

		vec3f n(0, -1.f, 0);
		vec3f pos3(pos.x, pos.y, 0);
		rect.addPackedVert(pos3 + vec3f(size.x, size.y, DEPTH2D), n, uvpos + uvsize, cols);
		rect.addPackedVert(pos3 + vec3f(0, 0, DEPTH2D), n, uvpos, cols);
		rect.addPackedVert(pos3 + vec3f(0, size.y, DEPTH2D), n, vec2f(uvpos.x, uvpos.y + uvsize.y), cols);

		rect.addPackedVert(pos3 + vec3f(size.x, 0, DEPTH2D), n, vec2f(uvpos.x + uvsize.x, uvpos.y), cols);
		rect.addPackedVert(pos3 + vec3f(0, 0, DEPTH2D), n, uvpos, cols);
		rect.addPackedVert(pos3 + vec3f(size.x, size.y, DEPTH2D), n, uvpos + uvsize, cols);

		rect.bbox.bboxFromVerts(rect.verts, 0, rect.vc, rect.stride);
		rect.materialRef = 0;
		return rect;
	}

	Pi3Cmesh rectLine(const vec2f &pos, const vec2f &size, const vec2f &uvpos, const vec2f &uvsize)
	{
		Pi3Cmesh rect("rectline");
		float *cols = nullptr;

		vec3f n(0, -1.f, 0);
		vec3f pos3(pos.x, pos.y, 0);
		rect.addPackedVert(pos3 + vec3f(0, 0, DEPTH2D), n, uvpos, cols);
		rect.addPackedVert(pos3 + vec3f(0, size.y, DEPTH2D), n, vec2f(uvpos.x, uvpos.y + uvsize.y), cols);
		rect.addPackedVert(pos3 + vec3f(size.x, size.y, DEPTH2D), n, uvpos + uvsize, cols);
		rect.addPackedVert(pos3 + vec3f(size.x, 0, DEPTH2D), n, vec2f(uvpos.x + uvsize.x, uvpos.y), cols);
		rect.addPackedVert(pos3 + vec3f(0, 0, DEPTH2D), n, uvpos, cols);
		rect.bbox.bboxFromVerts(rect.verts, 0, rect.vc, rect.stride);
		rect.materialRef = 0;
		return rect;
	}

	Pi3Cmesh plane(const vec3f &pos, const vec2f &size, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f &uvsize)
	{
		Pi3Cmesh plane("Plane");
		float *cols = nullptr;

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
				plane.addPackedVert(pos + vec, n, vec2f(xu + ux, yu + uy), cols);
				v[a] = x; v[b] = y + yd; memcpy(&vec, &v, 12);
				plane.addPackedVert(pos + vec, n, vec2f(xu, yu + uy), cols);
				v[a] = x; v[b] = y; memcpy(&vec, &v, 12);
				plane.addPackedVert(pos + vec, n, vec2f(xu, yu), cols);

				v[a] = x + xd; v[b] = y; memcpy(&vec, &v, 12);
				plane.addPackedVert(pos + vec, n, vec2f(xu + ux, yu), cols);
				v[a] = x + xd; v[b] = y + yd; memcpy(&vec, &v, 12);
				plane.addPackedVert(pos + vec, n, vec2f(xu + ux, yu + uy), cols);
				v[a] = x; v[b] = y; memcpy(&vec, &v, 12);
				plane.addPackedVert(pos + vec, n, vec2f(xu, yu), cols);
			}
		}

		plane.bbox.bboxFromVerts(plane.verts, 0, plane.vc, plane.stride);
		plane.materialRef = 0;
		plane.dynamic = true;
		return plane;
	}

	const float cubeOffs[72] = { -1.f,1.f,-1.f, 1.f,1.f,-1.f, 1.f,-1.f,-1.f, -1.f,-1.f,-1.f,
		1.f,1.f,-1.f, 1.f,1.f,1.f, 1.f,-1.f,1.f, 1.f,-1.f,-1.f,
		1.f,1.f,1.f, -1.f,1.f,1.f, -1.f,-1.f,1.f, 1.f,-1.f,1.f,
		-1.f,1.f,1.f, -1.f,1.f,-1.f, -1.f,-1.f,-1.f, -1.f,-1.f,1.f,
		-1.f,-1.f,-1.f, 1.f,-1.f,-1.f, 1.f,-1.f,1.f, -1.f,-1.f,1.f,
		1.f,1.f,-1.f, -1.f,1.f,-1.f, -1.f,1.f,1.f, 1.f,1.f,1.f };

	const float cnorms[18] = { 0,0,-1.0f, 1.0f,0,0, 0,0,1.0f, -1.0f,0,0, 0,-1.0f,0, 0,1.0f,0 };

	Pi3Cmesh cuboid(const vec3f &pos, const vec3f &size, const int divx, const int divy, const int divz)
	{
		Pi3Cmesh cube("Cuboid");
		float *cols = nullptr;
		vec3f hsize = size / 2.f;
		for (int i = 0; i < 6; i++)
		{
			int v = i * 12;
			vec3f n = vec3f(cnorms[i * 3], cnorms[i * 3 + 1], cnorms[i * 3 + 2]);
			cube.addPackedVert(pos + vec3f(hsize.x*cubeOffs[v + 6], hsize.y*cubeOffs[v + 7], hsize.z*cubeOffs[v + 8]), n, vec2f(1, 1), cols);
			cube.addPackedVert(pos + vec3f(hsize.x*cubeOffs[v + 3], hsize.y*cubeOffs[v + 4], hsize.z*cubeOffs[v + 5]), n, vec2f(0, 1), cols);
			cube.addPackedVert(pos + vec3f(hsize.x*cubeOffs[v], hsize.y*cubeOffs[v + 1], hsize.z*cubeOffs[v + 2]), n, vec2f(0, 0), cols);

			cube.addPackedVert(pos + vec3f(hsize.x*cubeOffs[v + 9], hsize.y*cubeOffs[v + 10], hsize.z*cubeOffs[v + 11]), n, vec2f(1, 0), cols);
			cube.addPackedVert(pos + vec3f(hsize.x*cubeOffs[v + 6], hsize.y*cubeOffs[v + 7], hsize.z*cubeOffs[v + 8]), n, vec2f(1, 1), cols);
			cube.addPackedVert(pos + vec3f(hsize.x*cubeOffs[v], hsize.y*cubeOffs[v + 1], hsize.z*cubeOffs[v + 2]), n, vec2f(0, 0), cols);
		}

		cube.bbox.bboxFromVerts(cube.verts, 0, cube.vc, cube.stride);
		cube.materialRef = 0;
		return cube;
	}
	
	void calcPathNormals(std::vector<vec2f> &path, std::vector<vec2f> &normals, float creaseAngle, bool joined, float inv)
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
		if (joined && sz>2) {
			p1 = path[sz - 2]; 
			if (angleBetweenTwoVectors2D(p2 - p1, p3 - p2) > 0.5f) {
				normals.push_back(dot(p2, p3)*inv);
			}
			else {
				normals.push_back(dot(p1 - p2, p3 - p2)*inv);
			}
			p1 = path[0];
		}
		else
		{
			p1 = path[0];
			normals.push_back(dot(p2, p3)*inv);
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

	void copyVerts(std::vector<float> &srcverts, std::vector<float> &destverts, uint32_t stride, uint32_t &vc, uint32_t ref)
	{
		if (vc + stride > destverts.size()) destverts.resize(vc + 1000);
		size_t p = ref*stride;
		memcpy(&destverts[vc], &srcverts[p], stride * sizeof(float));
		vc += stride;
		//for (size_t r = 0; r < stride; r++) destverts[vc++] = srcverts[p + r];
	}

	void addPackedVert(std::vector<float> &verts, uint32_t &vc, int32_t stride, vec3f position, vec3f normal, vec2f uv, GLfloat *cols)
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
		if (cols && stride == 12) {
			verts[vc++] = cols[0];
			verts[vc++] = cols[1];
			verts[vc++] = cols[2];
			verts[vc++] = cols[3];
		}
	}

	Pi3Cmesh disc(vec3f pos, float innerRad, float outerRad, int sides)
	{
		Pi3Cmesh model("ring");

		float rs = 2 * PI / (float)sides;
		float ux = 10.f / sides;
		std::vector<float> vertTemp;
		uint32_t tc = 0;
		float *cols = nullptr;
		uint32_t stride = 8;

		for (int r = 0; r < sides; r++) {
			float a = rs * (float)r;
			addPackedVert(vertTemp, tc, stride, pos + vec3f(cos(a)*innerRad, 0, sin(a)*innerRad), vec3f(0, 1.f, 0), vec2f(float(r)*ux, 0.999f), cols);
		}

		for (int r = 0; r < sides; r++) {
			float a = rs * (float)r;
			addPackedVert(vertTemp, tc, stride, pos + vec3f(cos(a)*outerRad, 0, sin(a)*outerRad), vec3f(0, 1.f, 0), vec2f(float(r)*ux, 0.f), cols);
		}

		model.stride = stride;

		for (int p = 0; p < sides; p++) {
			//GL_TRIANGLES
			copyVerts(vertTemp, model.verts, stride, model.vc, sides + p);
			copyVerts(vertTemp, model.verts, stride, model.vc, (p + 1) % sides);
			copyVerts(vertTemp, model.verts, stride, model.vc, p);

			copyVerts(vertTemp, model.verts, stride, model.vc, sides + p);
			copyVerts(vertTemp, model.verts, stride, model.vc, sides + (p + 1) % sides);
			copyVerts(vertTemp, model.verts, stride, model.vc, (p + 1) % sides);
		}

		model.bbox.bboxFromVerts(model.verts, 0, model.vc, model.stride);
		model.mode = GL_TRIANGLES;
		return model;

	}

	Pi3Cmesh lathe(std::string name, vec3f pos, std::vector<vec2f> &path, int edges, float rise, float startAngle, float endAngle, bool invert, float creaseAngle, vec2f prevPoint, vec2f lastPoint, uint32_t uvtype)
	{
		Pi3Cmesh model(name);
		
				/*
		float cols[4];
		cols[0] = (col & 255) / 255.f;
		cols[1] = ((col >> 8) & 255) / 255.f;
		cols[2] = ((col >> 16) & 255) / 255.f;
		cols[3] = ((col >> 24) & 255) / 255.f;
		*/

		float *cols = nullptr;
		uint32_t stride = 8;

		size_t sz = path.size();
		if (sz < 2) return model;

		float inv = (invert) ? -1.0f : 1.0f;
		std::vector<vec2f> normals;
		calcPathNormals(path, normals, 60, true, inv);
		sz = path.size();

		float angDiff = endAngle - startAngle;
		float angStep = angDiff / (float)edges;
		//int32_t rl = (int32_t)(sides*coils);
		
		float tcy = 0.0f;
		float tcx = 1.0f / (float)edges;
		//float pr = (PI / (float)edges) * 2.f;
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
				addPackedVert(vertTemp, tc, stride, pos + vec3f(path[p].x*sinr, risey, path[p].x*cosr), vec3f(sinr*normals[p].x, normals[p].y, cosr*normals[p].x), vec2f(tcx*r, tcy), cols);
				risey += rdiv;
			}

			addPackedVert(vertTemp, tc, stride, pos + vec3f(0, path[p].y, path[p].x), vec3f(0, normals[p].y, normals[p].x), vec2f(0.9999f, tcy), cols);
		}

		model.stride = stride;

		int pn = 0;
		int pp = 0;
		for (size_t p = 0; p < (sz-1); p++) {
			pn += (edges + 1);
			if ((path[p + 1] != path[p]) || p==(sz-2)) {
				for (int32_t r = 0; r < edges; r++)
				{
					//GL_TRIANGLES
					copyVerts(vertTemp, model.verts, stride, model.vc, pn + r);
					copyVerts(vertTemp, model.verts, stride, model.vc, pp + r + 1);
					copyVerts(vertTemp, model.verts, stride, model.vc, pp + r);

					copyVerts(vertTemp, model.verts, stride, model.vc, pn + r + 1);
					copyVerts(vertTemp, model.verts, stride, model.vc, pp + r + 1);
					copyVerts(vertTemp, model.verts, stride, model.vc, pn + r);
				}
			}
			pp += (edges + 1);
		}

		model.bbox.bboxFromVerts(model.verts, 0, model.vc, model.stride);
		model.mode = GL_TRIANGLES;
		return model;
	}

	Pi3Cmesh sphere(vec3f pos, float radius, float hemi, int slices, int sides, bool invert)
	{
		std::vector<vec2f> path;
		float st = (PI*(1.0f - hemi)) / static_cast<float>(slices);
		float ho = PI - (PI*(1.f - hemi));
		float inv = invert ? -1.0f : 1.0f;
		for (int r = 0; r <= slices; r++) {
			path.push_back(vec2f(radius*-sin(static_cast<float>(r)*st+ho)*inv, radius*-cos(static_cast<float>(r)*st+ho)));
		}
		return lathe("Sphere", pos, path, sides, 0.0f, 0.0f, 2.f*PI, false, 60.f, vec2f(1, path[0].y), vec2f(-1, path[path.size() - 1].y),UVMAP_SPHERE);
	}

	Pi3Cmesh torus(vec3f pos, float radius, float ringradius, int ringrots, int sides)
	{
		std::vector<vec2f> path;
		float st = PI2 / (float)ringrots;
		for (int r = 0; r < (ringrots + 1); r++) {
			path.push_back(vec2f(radius + ringradius*sinf(-r*st), ringradius*cosf(-r*st)));
		}
		return lathe("Torus", pos, path, sides, 0.0f, 0.0f, PI2, false, 60.f);
	}

	Pi3Cmesh spring(vec3f pos, float radius, float length, int coils, float coilradius, int ringrots, int sides)
	{
		std::vector<vec2f> path;
		//float rise = length / coils;
		float st = (PI * 2.f) / (float)ringrots;
		for (int r = 0; r < (ringrots + 1); r++) {
			path.push_back(vec2f(radius + coilradius*sin(r*st), coilradius*cos(r*st)));
		}
		return lathe("Spring",pos, path, sides, length, 0.0f, 2.f*PI*(float)coils);
	}

	Pi3Cmesh cone(vec3f pos, float radius, float height, int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(0, height));
		path.push_back(vec2f(radius, 0));
		path.push_back(vec2f(0, 0));
		return lathe("Cone",pos, path, sides, 0.0f, 0.0f, 2.f*PI, true, 60.0f, vec2f(-radius, height*2), vec2f(-radius, 0));
	}

	Pi3Cmesh tcone(vec3f pos, float botradius, float topradius, float height, int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(0, height));
		path.push_back(vec2f(topradius, height));
		path.push_back(vec2f(botradius, 0));
		path.push_back(vec2f(0, 0));
		return lathe("Truncated Cone", pos, path, sides, 0.f, 0.0f, 2.f*PI, true, 60.0f, vec2f(-1.0f,height));
	}

	Pi3Cmesh cylinder(vec3f pos, float radius, float height, int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(0, height));
		path.push_back(vec2f(radius, height));
		path.push_back(vec2f(radius, 0));
		path.push_back(vec2f(0, 0));
		return lathe("Cylinder", pos, path, sides, 0.0f, 0.0f, 2.f*PI, true, 60.0f, vec2f(-1.0f, height));
	}

	Pi3Cmesh tube( vec3f pos, float radius1, float radius2, float height, int sides)
	{
		std::vector<vec2f> path;
		path.push_back(vec2f(radius1, height));
		path.push_back(vec2f(radius2, height));
		path.push_back(vec2f(radius2, 0));
		path.push_back(vec2f(radius1, 0));
		path.push_back(vec2f(radius1, height));
		return lathe( "Tube", pos, path, sides, 0.0f, 0.0f, 2.f*PI, true, 60.0f, vec2f(0, 0), vec2f(0, 0), UVMAP_SPHERE);
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


	Pi3Cmesh extrude(std::string name, vec3f pos, std::vector<std::vector<vec2f>> &contoursxy, float depth, uint16_t divs)
	{
		//Convert vec2f to floats ... (yes, I know, but I cant be bothered to vec2f tesselate for now!)
		std::vector<std::vector<float>> contours;
		for (size_t i = 0; i < contoursxy.size(); i++) {
			contours.emplace_back();
			std::vector<vec2f> &contourxy = contoursxy[i];
			std::vector<float> &contour = contours.back();
			contour.resize(contourxy.size() * 2);
			memcpy(&contour[0], &contourxy[0], contourxy.size() * 2 * sizeof(float));
		}

		//Find leftmost polygon (always an outside polygon)
		float leftx = 1e20f;
		int f = -1;
		for (size_t c = 0; c < contours.size(); c++) {
			std::vector<float> &contour = contours[c];
			for (size_t p = 0; p < contour.size(); p += 2) {
				if (contour[p] < leftx) {
					leftx = contour[p];
					f = c;
				}
			}
		}

		//Work out CW/CCW of leftmost polygon ...
		std::vector<float> &contour = contours[f];
		float a = 0.f; size_t p = contour.size() - 2;
		for (size_t q = 0; q < contour.size() - 1; q += 2) {
			a += contour[p] * contour[q + 1] - contour[q] * contour[p + 1];
			p = q;
		}

		//reverse contours if inverted
		if (a > 0)
		{
			for (size_t a = 0; a < contours.size(); a++) {
				std::vector<float> &contour = contours[a];
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

		Pi3Cmesh extrusion(name);

		//float cols[4];
		//cols[0] = (col & 255) / 255.f;
		//cols[1] = ((col >> 8) & 255) / 255.f;
		//cols[2] = ((col >> 16) & 255) / 255.f;
		//cols[3] = ((col >> 24) & 255) / 255.f;

		float *cols = nullptr;

		//extrusion.stride = 8;
		//extrusion.vertOffset = 0; // vc / extrusion.stride;
		vec3f n = vec3f(0.f, 1.f, 0.f);
		for (int i = 0; i < triCount; i++) {  //Top faces
			int t = i * 9;
			extrusion.addPackedVert(pos + vec3f(tris[t], depth, tris[t + 1]), n, vec2f(0, 0), cols);
			extrusion.addPackedVert(pos + vec3f(tris[t + 6], depth, tris[t + 7]), n, vec2f(0, 0), cols);
			extrusion.addPackedVert(pos + vec3f(tris[t + 3], depth, tris[t + 4]), n, vec2f(0, 0), cols);
		}
		n = vec3f(0.f, -1.f, 0.f);
		for (int i = 0; i < triCount; i++) {  //Bottom faces
			int t = i * 9;
			extrusion.addPackedVert(pos + vec3f(tris[t], 0, tris[t + 1]), n, vec2f(0, 0), cols);
			extrusion.addPackedVert(pos + vec3f(tris[t + 3], 0, tris[t + 4]), n, vec2f(0, 0), cols);
			extrusion.addPackedVert(pos + vec3f(tris[t + 6], 0, tris[t + 7]), n, vec2f(0, 0), cols);
		}

		for (size_t i = 0; i < edges.size(); i++) {
			std::vector<float> &contour = edges[i];
			for (size_t j = 0; j < contour.size(); j+=2) {
				size_t k = (j + 2) % contour.size();
				vec2f norm = dot(vec2f(contour[j], contour[j + 1]), vec2f(contour[k], contour[k + 1]));
				extrusion.addPackedVert(pos + vec3f(contour[j], 0, contour[j + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), cols);
				extrusion.addPackedVert(pos + vec3f(contour[k], depth, contour[k + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), cols);
				extrusion.addPackedVert(pos + vec3f(contour[k], 0, contour[k + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), cols);

				extrusion.addPackedVert(pos + vec3f(contour[k], depth, contour[k + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), cols);
				extrusion.addPackedVert(pos + vec3f(contour[j], 0, contour[j + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), cols);
				extrusion.addPackedVert(pos + vec3f(contour[j], depth, contour[j + 1]), vec3f(norm.x, 0, norm.y), vec2f(0, 0), cols);
			}
		}

		//extrusion.vertSize = extrusion.vc / extrusion.stride - extrusion.vertOffset;
		extrusion.bbox.bboxFromVerts(extrusion.verts, 0, extrusion.vc, extrusion.stride);
		extrusion.mode = GL_TRIANGLES;

		texMap(extrusion.verts, extrusion.vc, extrusion.stride, 6,0,2);
		extrusion.materialRef = 0;
		return extrusion;
	}

	vec3f unitNormal(vec3f v0, vec3f v1, vec3f v2) {
		return ((v1 - v0) ^ (v2 - v0)).unit();
	}

	Pi3Cmesh elevationMap(uint8_t * map, uint32_t w, uint32_t h, uint32_t bpp, vec3f pos, vec2f size, float mapHeight, const uint32_t xdivs, const uint32_t ydivs, int direction, const vec2f uvsize)
	{
		Pi3Cmesh meshmap("Map");
		float *cols = nullptr;

		uint32_t mw = w * bpp; //byte width of map

		vec3f vec1,vec2,vec3,vec4,vec5;
		float v1[3], v2[3], v3[3], v4[3], v5[3];
		int a = 0; int b = 1; int c = 2;
		vec3f n(0, 0, -1.f);

		switch (direction) {
		case 0: size.x = -size.x; break;
		case 1: n = vec3f(0, 0, 1.f); break;
		case 2: a = 2; b = 1; c = 0; n = vec3f(-1.f, 0, 0); break;
		case 3: a = 2; b = 1; c = 0; n = vec3f(1.f, 0, 0); size.x = -size.x; break;
		case 4: a = 0; b = 2; c = 1; n = vec3f(0, -1.f, 0); break;
		case 5: a = 0; b = 2; c = 1; n = vec3f(0, 1.f, 0); size.x = -size.x; break;
		}

		const float xs = 1.f / static_cast<float>(xdivs);
		const float ys = 1.f / static_cast<float>(ydivs);
		const float xd = xs * size.x;
		const float yd = ys * size.y;
		const float ux = xs * uvsize.x;
		const float uy = ys * uvsize.y;

		for (float yi = -0.5f; yi < 0.499f; yi += ys) {
			for (float xi = -0.5f; xi < 0.499f; xi += xs) {

				float x = xi * size.x; float y = yi * size.y;
				float xu = xi * uvsize.x; float yu = yi * uvsize.y;

				float h00 = ((float)(map[(uint32_t)((float)w*xu)*bpp + ((uint32_t)((float)h*yu))*mw]) / 255.f) * mapHeight;
				float h10 = ((float)(map[(uint32_t)((float)w*(xu + ux))*bpp + ((uint32_t)((float)h*yu))*mw]) / 255.f) * mapHeight;
				float h01 = ((float)(map[(uint32_t)((float)w*xu)*bpp + ((uint32_t)((float)h*(yu + uy)))*mw]) / 255.f) * mapHeight;
				float h11 = ((float)(map[(uint32_t)((float)w*(xu + ux))*bpp + ((uint32_t)((float)h*(yu + uy)))*mw]) / 255.f) * mapHeight;
				float hh = (h00 + h01 + h10 + h11) / 4.0f; //avg height for centre point

				v1[a] = x + xd; v1[b] = y + yd; v1[c] = h11; memcpy(&vec1, &v1, 12);
				v2[a] = x; v2[b] = y; v2[c] = h00; memcpy(&vec2, &v2, 12);
				v3[a] = x; v3[b] = y + yd; v3[c] = h01; memcpy(&vec3, &v3, 12);
				v4[a] = x + xd; v4[b] = y; v4[c] = h10; memcpy(&vec4, &v4, 12);
				v5[a] = x + xd*0.5f; v5[b] = y + yd*0.5f; v5[c] = hh; memcpy(&vec5, &v4, 12);

				n = unitNormal(vec5, vec2, vec3);
				meshmap.addPackedVert(pos + vec5, n, vec2f(xu + ux*0.5f, yu + uy * 0.5f), cols);
				meshmap.addPackedVert(pos + vec2, n, vec2f(xu, yu), cols);
				meshmap.addPackedVert(pos + vec3, n, vec2f(xu, yu + uy), cols);

				n = unitNormal(vec5, vec3, vec1);
				meshmap.addPackedVert(pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), cols);
				meshmap.addPackedVert(pos + vec3, n, vec2f(xu, yu + uy), cols);
				meshmap.addPackedVert(pos + vec1, n, vec2f(xu + ux, yu + uy), cols);

				n = unitNormal(vec5, vec1, vec4);
				meshmap.addPackedVert(pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), cols);
				meshmap.addPackedVert(pos + vec1, n, vec2f(xu + ux, yu + uy), cols);
				meshmap.addPackedVert(pos + vec4, n, vec2f(xu + ux, yu), cols);

				n = unitNormal(vec5, vec4, vec2);
				meshmap.addPackedVert(pos + vec5, n, vec2f(xu + ux * 0.5f, yu + uy * 0.5f), cols);
				meshmap.addPackedVert(pos + vec4, n, vec2f(xu + ux, yu), cols);
				meshmap.addPackedVert(pos + vec2, n, vec2f(xu, yu), cols);
			}
		}

		meshmap.bbox.bboxFromVerts(meshmap.verts, 0, meshmap.vc, meshmap.stride);
		meshmap.materialRef = 0;
		meshmap.dynamic = true;
		return meshmap;
	}

	Pi3Cmesh createGeoSphere(int stacks = 32, int slices = 32) {
		Pi3Cmesh mesh;
		float *cols = nullptr;

		//add vertices
		mesh.addPackedVert(vec3f(0, 1, 0), vec3f(0,1,0),vec2f(0,0.5f),cols);

		for (int i = 1; i < stacks; i++) {
			double phi = PI * double(i) / stacks;
			double cosPhi = cos(phi);
			double sinPhi = sin(phi);
			for (int j = 0; j < slices; j++) {
				double theta = PI*2 * double(j) / slices;
				mesh.addPackedVert(vec3f((float)(cos(theta)*sinPhi), (float)cosPhi, (float)(sin(theta)*sinPhi)), vec3f(0, 1.f, 0), vec2f(0, 0.5f), cols);
			}
		}
		mesh.addPackedVert(vec3f(0, -1.f, 0), vec3f(0, -1.f, 0), vec2f(1.f, 0.5f), cols);

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

	
}
