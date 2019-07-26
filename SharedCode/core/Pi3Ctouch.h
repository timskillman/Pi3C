#pragma once
#include "Pi3Cvector.h"

class Pi3Cmodel;	//forward declare Pi3Cmodel (so we dont get interdependencies)

class Pi3Ctouch {
public:
	Pi3Ctouch() {}
	~Pi3Ctouch() {}

	//functions
	void calcRay(vec3f touchPoint, float perspective);
	bool touched() { return triRef >= 0; }

	//input
	vec3f touchPoint;
	vec3f raypos;
	vec3f raydir;

	//output
	int32_t groupRefs[100];
	int32_t maxlevel;
	//Pi3Cmodel *selgroup;
	Pi3Cmodel *selmodel;
	int32_t meshRef;
	int32_t triRef;
	vec3f intersection;
	vec3f normal;

	//temps
	bool touching;
	float perspective;
	float prevDist;			//temp var used by mesh touch
};