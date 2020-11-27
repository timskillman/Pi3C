#pragma once
#include "Pi3Cvector.h"
#include "Pi3Crect.h"
#include "Pi3Cmatrix.h"

class viewInfo {
public:

	enum Projection { PERSPECTIVE, ORTHOGRAPHIC };
	enum SceneLayout { INACTIVE = -1, TOPLEFT = 0, TOPRIGHT = 1, BOTTOMLEFT = 2, BOTTOMRIGHT = 3, FULL = 4, FULLSCREEN = 5 };
	enum ViewProject { VIEW_LEFT, VIEW_RIGHT, VIEW_TOP, VIEW_BOTTOM, VIEW_FRONT, VIEW_BACK, VIEW_FREE, VIEW_PERSPECTIVE };

	void setRotMatrix(const vec3f &rot) {
		rotInvertMatrix.setRotateXY(rot);
		rotInvertMatrix = rotInvertMatrix.inverse();
	}

	void setRot(const vec3f &rot) {
		setRotMatrix(rot);
		this->rot = rot;
	}

	void addRot(const vec3f &dir)
	{
		rot += dir;
		setRotMatrix(rot);
	}

	vec3f viewCoords(const vec3f& pos)
	{
		setRotMatrix(rot);
		float iz = (projection == ORTHOGRAPHIC) ? 2.f / zoom : zoom / pspvalue;
		return rotInvertMatrix.transformRotateVec(pos*iz);
	}

	/* calculate orthographic position of mouse in scene .. */
	vec3f calcMouseXYZ(const int mx, const int my) {
		vec3f cpos((float)(-(mx - viewport.x - viewport.width / 2)), (float)(-(my - viewport.y - viewport.height / 2)), 0);
		return viewCoords(cpos) + pan;
	}

	float zoomFactor() {
		return (projection == ORTHOGRAPHIC) ? zoom / 10.f : -5.f;
	}

	ViewProject viewProject;
	Projection projection = PERSPECTIVE;
	Pi3Crecti viewport;
	Pi3Cmatrix rotInvertMatrix;
	vec3f pos{ 0,0,0 };
	vec3f rot{ 0,0,0 };
	vec3f pan{ 0,0,0 };
	float psp_nearz = 0.2f;
	float psp_farz = 20000.f;
	float ortho_nearz = -2000.f;
	float ortho_farz = 2000.f;
	float zoom = 100.f;
	float pspvalue = 600.f;
	float ticks = 1.f;
};