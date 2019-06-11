#include "Pi3Cmesh.h"
#include "Pi3Cresource.h"
#include <vector>

class Pi3CspriteArray {
public:
	void addSprite(const vec3f &pos, const vec2f &size, const float angle = 0, const vec2f &uv = vec2f(0, 0), const vec2f &us = vec2f(1.f, 1.f));
	void updateSpriteCoords(Pi3Cresource *resource, const uint32_t spriteRef, const float x, const float y);
	void updateSpriteCoordsRotated(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const std::vector<float> &angles);
	void updateSpriteTransformCoords(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent); //const vec2f &uv, const vec2f &us,
	void updateSpriteBillboard(Pi3Cresource *resource, const std::vector<vec3f> &pos, const std::vector<vec2f> &size, const vec3f &lookat);

	int32_t meshRef;	//ref to final mesh in resources
	Pi3Cmesh samesh;		//temp mesh
};
