#include "Pi3CspriteArray.h"

void Pi3CspriteArray::addSprite(const vec3f &pos, const vec2f &size, const float angle, const vec2f &uv, const vec2f &us)
{
	samesh.addRect(pos, size, uv, us);
}

void Pi3CspriteArray::updateSpriteCoords(Pi3Cresource *resource, const uint32_t spriteRef, const float x, const float y)
{
	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t p = mesh.calcRectPtr(spriteRef);
	std::vector<float> &verts = resource->vertBuffer[resource->meshes[meshRef].bufRef];

	float w = verts[p + mesh.stride] - verts[p];					//derive width and height from rectangle
	float h = verts[p + mesh.stride + 1] - verts[p + 1];

	mesh.updateRectCoords2D(verts, p, x, y, w, h);
	//updateCoordsXY(x, y - h);
	//updateCoordsXY(x + w, y);
	//updateCoordsXY(x + w, y - h);
	//updateCoordsXY(x, y - h);
	//updateCoordsXY(x, y);
	//updateCoordsXY(x + w, y);
}

void Pi3CspriteArray::updateSpriteCoordsRotated(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const std::vector<float> &angle)
{

	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t p = mesh.calcRectPtr(0);
	std::vector<float> &verts = resource->vertBuffer[resource->meshes[meshRef].bufRef];

	for (size_t i = 0; i < pos.size(); i++) {

		float hw = size[i].x * 0.5f;
		float hh = size[i].y * 0.5f;

		float ca = cos(angle[i]);
		float sa = sin(angle[i]);
		float ax = hw * ca - hh * sa;
		float ay = hw * sa + hh * ca;

		mesh.updateRectCoordsROT(verts, p, pos[i].x, pos[i].y, ax, ay);

		//updateRotateCoordsXY(-ax, ay);
		//updateRotateCoordsXY(ax, -ay);
		//updateRotateCoordsXY(-ay, -ax);
		//updateRotateCoordsXY(-ax, ay);
		//updateRotateCoordsXY(ay, ax);
		//updateRotateCoordsXY(ax, -ay);
	}
	resource->updateMesh(meshRef);
}

void Pi3CspriteArray::updateSpriteBillboard(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const vec3f &lookat)
{
	//Transform pos into screen space ...
	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t p = mesh.calcRectPtr(0);
	std::vector<float> *verts = &(&resource->vertBuffer[resource->meshes[meshRef].bufRef])[p];

	for (size_t i = 0; i < pos.size(); i++) {
		float hw = size[i].x * 0.5f;
		//float hh = size.y * 0.5f;

		float lx = lookat.x - pos[i].x;
		float lz = lookat.z - pos[i].z;
		float d = hw / sqrtf(lx*lx + lz * lz);
		float ax = lz * d, az = -lx * d;

		mesh.updateRectCoordsBB(*verts, p, pos[i], ax, az, size[i].y);
	}
	resource->updateMesh(meshRef);
}


/* Transforms a 2D rect into 3D space with the effect of bill-boarding */
void Pi3CspriteArray::updateSpriteTransformCoords(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent) //, const vec2f &uv, const vec2f &us
{
	//Transform pos into screen space ...
	Pi3Cmesh &mesh = resource->meshes[meshRef];
	uint32_t p = mesh.calcRectPtr(0);
	std::vector<float> &verts = resource->vertBuffer[resource->meshes[meshRef].bufRef];

	for (size_t i = 0; i < pos.size(); i++) {

		vec3f tpos = scene_matrix->transformVec(pos[i]);
		float hw = size[i].x * 0.5f;
		//float hh = size.y * 0.5f;
		float cx = cent.x, cy = cent.y;
		float w = 800.f / tpos.z;

		mesh.updateRectCoordsPSP(verts, p, tpos.x, tpos.y, tpos.z, cx, cy, hw, size[i].y, w);

		//updateCoordsXYZ(cx - (tpos.x - hw)*w, cy - tpos.y*w, tpos.z); // , uv.x, uv.y);
		//updateCoordsXYZ(cx - (tpos.x + hw)*w, cy - (tpos.y + size.y)*w, tpos.z); // , uv.x + us.x, uv.y);
		//updateCoordsXYZ(cx - (tpos.x + hw)*w, cy - tpos.y*w, tpos.z); // , uv.x + us.x, uv.y + us.y);
		//updateCoordsXYZ(cx - (tpos.x - hw)*w, cy - tpos.y*w, tpos.z); // , uv.x, uv.y);
		//updateCoordsXYZ(cx - (tpos.x - hw)*w, cy - (tpos.y + size.y)*w, tpos.z); // , uv.x, uv.y + us.y);
		//updateCoordsXYZ(cx - (tpos.x + hw)*w, cy - (tpos.y + size.y)*w, tpos.z); // , uv.x + us.x, uv.y + us.y);
	}
	resource->updateMesh(meshRef);
}

