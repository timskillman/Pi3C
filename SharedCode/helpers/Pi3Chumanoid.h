#pragma once
#include "Pi3Cmodel.h"
#include <string>

class Pi3Chumanoid {
public:

	struct humanoidParams {
		vec3f headSize = vec3f(2.f, 2.2f, 2.2f);
		vec3f torsoSize = vec3f(3.f, 3.8f, 1.5f);
		vec3f pelvisSize = vec3f(2.7f, 1.5f, 1.4f);
		vec3f upperArmsSize = vec3f(0.8f, 3.2f, 0.8f);
		vec3f lowerArmsSize = vec3f(0.7f, 3.2f, 0.6f);
		vec3f handSize = vec3f(0.5f, 0.8f, 0.2f);
		vec3f thighSize = vec3f(1.2f, 3.5f, 1.f);
		vec3f lowerLegsSize = vec3f(1.0f, 3.5f, 1.f);
		vec3f footSize = vec3f(0.9f, 0.4f, 1.8f);

		//Set any of these file strings to load OBJ geometry in place of basic geometry ...
		std::string head_file;
		std::string torso_file;
		std::string pelvis_file;
		std::string lthigh_file;
		std::string rthigh_file;
		std::string llowerleg_file;
		std::string rlowerleg_file;
		std::string lfoot_file;
		std::string rfoot_file;
		std::string lupperArm_file;
		std::string rupperArm_file;
		std::string llowerArm_file;
		std::string rlowerArm_file;
		std::string lhand_file;
		std::string rhand_file;
	};

	Pi3Chumanoid() : walkPos(0) {}

	//Pi3Cmodel body;
	float walkPos;

	Pi3Cmodel* head;
	Pi3Cmodel* torso;
	Pi3Cmodel* pelvis;

	Pi3Cmodel* leftUpperArm;
	Pi3Cmodel* leftLowerArm;
	Pi3Cmodel* leftHand;

	Pi3Cmodel* rightUpperArm;
	Pi3Cmodel* rightLowerArm;
	Pi3Cmodel* rightHand;

	Pi3Cmodel* leftFoot;
	Pi3Cmodel* leftLowerLeg;
	Pi3Cmodel* leftThigh;

	Pi3Cmodel* rightFoot;
	Pi3Cmodel* rightLowerLeg;
	Pi3Cmodel* rightThigh;

	Pi3Cmodel create(Pi3Cresource *resource, humanoidParams &params);

	void walk(vec3f dir, float speed);
	void attachModel(Pi3Cmodel &body);

private:
	Pi3Cmesh loadMesh(const std::string &file, Pi3Cresource *resource, const vec3f &pivot, const vec3f &size);

};