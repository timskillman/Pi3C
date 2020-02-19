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
	vec3f touchPoint = { 0, 0, 0 };
	vec3f raypos = { 0, 1.f, 0 };
	vec3f raydir = { 0, 1.f, 0 };

	//output
	int32_t groupRefs[100];
	int32_t maxlevel = 0;
	//Pi3Cmodel *selgroup;
	Pi3Cmodel *selmodel = nullptr;
	int32_t meshRef = -1;
	int32_t triRef = -1;
	vec3f intersection = { 0, 0, 0 };
	vec3f normal = { 0, 0, 0 };

	//temps
	bool touching = false;
	float perspective =  400.f;
	float prevDist = 0;			//temp var used by mesh touch
};