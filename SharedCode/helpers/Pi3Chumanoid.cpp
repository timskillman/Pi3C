#include "Pi3Chumanoid.h"
#include "Pi3CfileOBJ.h"
#include "Pi3Cshapes.h"

Pi3Cmesh Pi3Chumanoid::loadMesh(const std::string &file, Pi3Cresource *resource, const vec3f &pivot, const vec3f &size)
{
	std::string error;
	size_t meshStart = resource->meshes.size();
	Pi3CfileOBJ::load("", file, resource, 0, false, false, 0, error);
	size_t meshEnd = resource->meshes.size();

	//just use first mesh in file (should really merge meshes first) ...
	Pi3Cmesh &mesh = resource->meshes[meshStart];
	mesh.resize(pivot, size, resource->vertBuffer[mesh.bufRef].verts);
	return mesh;			
}

Pi3Cmodel Pi3Chumanoid::create(Pi3Cresource *resource, humanoidParams &hp, int32_t groupId)
{
	Pi3Cmodel body;

	//create arms ...
	vec3f handPivot(0, -hp.handSize.y*0.5f, 0);
	vec3f lowArmPivot(0, -hp.lowerArmsSize.y*0.5f, 0);
	vec3f upArmPivot(0, -hp.upperArmsSize.y*0.5f, 0);

	Pi3Cmesh lhand = (hp.lhand_file == "") ? Pi3Cshapes::cuboid(handPivot, hp.handSize) : loadMesh(hp.lhand_file, resource, handPivot, hp.handSize);
	Pi3Cmodel mlhand = Pi3Cmodel(resource, "leftHand", lhand, groupId);

	Pi3Cmesh llowarm = (hp.llowerArm_file == "") ? Pi3Cshapes::cuboid(lowArmPivot, hp.lowerArmsSize) : loadMesh(hp.llowerArm_file, resource, lowArmPivot, hp.lowerArmsSize);
	Pi3Cmodel mllowerArm = Pi3Cmodel(resource, "leftLowerArm", llowarm, groupId);
	mllowerArm.append(resource, mlhand, vec3f(0, -hp.lowerArmsSize.y, 0), vec3f());
	mllowerArm.rotate(vec3f(-1.57f, 0, 0));
	
	Pi3Cmesh luparm = (hp.lupperArm_file == "") ? Pi3Cshapes::cuboid(upArmPivot, hp.upperArmsSize) : loadMesh(hp.lupperArm_file, resource, upArmPivot, hp.upperArmsSize);
	Pi3Cmodel mlupperArm = Pi3Cmodel(resource, "leftUpperArm", luparm, groupId);
	mlupperArm.append(resource, mllowerArm, vec3f(0, -hp.upperArmsSize.y, 0), vec3f());
	mlupperArm.rotate(vec3f(0.5f, 0, 0));

	Pi3Cmesh rhand = (hp.rhand_file == "") ? Pi3Cshapes::cuboid(handPivot, hp.handSize) : loadMesh(hp.rhand_file, resource, handPivot, hp.handSize);
	Pi3Cmodel mrhand = Pi3Cmodel(resource, "rightHand", rhand, groupId);

	Pi3Cmesh rlowarm = (hp.rlowerArm_file == "") ? Pi3Cshapes::cuboid(lowArmPivot, hp.lowerArmsSize) : loadMesh(hp.rlowerArm_file, resource, lowArmPivot, hp.lowerArmsSize);
	Pi3Cmodel mrlowerArm = Pi3Cmodel(resource, "rightLowerArm", rlowarm, groupId);
	mrlowerArm.append(resource, mrhand, vec3f(0, -hp.lowerArmsSize.y, 0), vec3f());

	Pi3Cmesh ruparm = (hp.rupperArm_file == "") ? Pi3Cshapes::cuboid(upArmPivot, hp.upperArmsSize) : loadMesh(hp.rupperArm_file, resource, upArmPivot, hp.upperArmsSize);
	Pi3Cmodel mrupperArm = Pi3Cmodel(resource, "rightUpperArm", ruparm, groupId);
	mrupperArm.append(resource, mrlowerArm, vec3f(0, -hp.upperArmsSize.y, 0), vec3f());

	//create legs ...
	vec3f footPivot(0, -hp.footSize.y*0.5f, hp.footSize.z*0.25f);
	vec3f lowLegPivot(0, -hp.lowerLegsSize.y*0.5f, 0);
	vec3f thighPivot(0, -hp.thighSize.y*0.5f, 0);

	Pi3Cmesh lfoot = (hp.lfoot_file == "") ? Pi3Cshapes::cuboid(footPivot, hp.footSize) : loadMesh(hp.lfoot_file, resource, footPivot, hp.footSize);
	Pi3Cmodel mlfoot = Pi3Cmodel(resource, "leftFoot", lfoot, groupId);

	Pi3Cmesh llowleg = (hp.llowerleg_file == "") ? Pi3Cshapes::cuboid(lowLegPivot, hp.lowerLegsSize) : loadMesh(hp.llowerleg_file, resource, lowLegPivot, hp.lowerLegsSize);
	Pi3Cmodel mllowerLeg = Pi3Cmodel(resource, "leftLowerLeg", llowleg, groupId);
	mllowerLeg.append(resource, mlfoot, vec3f(0, -hp.lowerLegsSize.y, 0), vec3f());
	mllowerLeg.rotate(vec3f(1.f, 0, 0));

	Pi3Cmesh lthigh = (hp.lthigh_file == "") ? Pi3Cshapes::cuboid(thighPivot, hp.thighSize) : loadMesh(hp.lthigh_file, resource, thighPivot, hp.thighSize);
	Pi3Cmodel mlthigh = Pi3Cmodel(resource, "leftThigh", lthigh, groupId);
	mlthigh.append(resource, mllowerLeg, vec3f(0, -hp.thighSize.y, 0), vec3f());
	mlthigh.rotate(vec3f(-0.5f, 0, 0));
	
	Pi3Cmesh rfoot = (hp.rfoot_file == "") ? Pi3Cshapes::cuboid(footPivot, hp.footSize) : loadMesh(hp.rfoot_file, resource, footPivot, hp.footSize);
	Pi3Cmodel mrfoot = Pi3Cmodel(resource, "rightFoot", rfoot, groupId);

	Pi3Cmesh rlowleg = (hp.rlowerleg_file == "") ? Pi3Cshapes::cuboid(lowLegPivot, hp.lowerLegsSize) : loadMesh(hp.rlowerleg_file, resource, lowLegPivot, hp.lowerLegsSize);
	Pi3Cmodel mrlowerLeg = Pi3Cmodel(resource, "rightLowerLeg", rlowleg, groupId);
	mrlowerLeg.append(resource, mrfoot, vec3f(0, -hp.lowerLegsSize.y, 0), vec3f());

	Pi3Cmesh rthigh = (hp.rthigh_file == "") ? Pi3Cshapes::cuboid(thighPivot, hp.thighSize) : loadMesh(hp.rthigh_file, resource, thighPivot, hp.thighSize);
	Pi3Cmodel mrthigh = Pi3Cmodel(resource, "rightThigh", rthigh, groupId);
	mrthigh.append(resource, mrlowerLeg, vec3f(0, -hp.thighSize.y,0), vec3f());

	// head ...
	Pi3Cmesh head = (hp.head_file == "") ? Pi3Cshapes::sphere(vec3f(0, 0, 0), 0.5f, 0.f, 10, 20) : loadMesh(hp.head_file, resource, vec3f(0, 0, 0),vec3f(1.f,1.f,1.f));
	Pi3Cmodel mhead = Pi3Cmodel(resource, "head", head, groupId);
	mhead.resize(hp.headSize);
	//mhead.move(vec3f(0, torsoSize.y + headSize.y*0.5f, 0));

	// torso and attach head and arms ...
	vec3f torsoPivot(0, hp.torsoSize.y *0.5f, 0);
	Pi3Cmesh torso = (hp.torso_file == "") ? Pi3Cshapes::cuboid(torsoPivot, hp.torsoSize) : loadMesh(hp.torso_file, resource, torsoPivot, hp.torsoSize);
	Pi3Cmodel mtorso = Pi3Cmodel(resource, "torso", torso, groupId);
	//mtorso.move(vec3f(0, torsoSize.y, 0));

	mtorso.append(resource, mhead, vec3f(0, hp.torsoSize.y + hp.headSize.y*0.5f, 0), vec3f());
	mtorso.append(resource, mlupperArm, vec3f(-(hp.torsoSize.x + hp.upperArmsSize.x) / 2.f, hp.torsoSize.y, 0), vec3f());
	mtorso.append(resource, mrupperArm, vec3f((hp.torsoSize.x + hp.upperArmsSize.x) / 2.f, hp.torsoSize.y, 0), vec3f());
	//mtorso.rotate(vec3f(0, 0.5f, 0));

	// pelvis and attach torso and legs ...
	vec3f pelvisPivot(0, hp.pelvisSize.y*0.5f, 0);
	Pi3Cmesh pelvis = (hp.pelvis_file == "") ? Pi3Cshapes::cuboid(pelvisPivot, hp.pelvisSize) : loadMesh(hp.pelvis_file, resource, pelvisPivot, hp.pelvisSize);
	Pi3Cmodel mpelvis = Pi3Cmodel(resource, "pelvis", pelvis, groupId);
	//mpelvis.move(vec3f(0, footSize.y + lowerLegsSize.y + thighSize.y, 0));
	mpelvis.append(resource, mtorso, vec3f(0, hp.pelvisSize.y, 0), vec3f());
	mpelvis.append(resource, mlthigh, vec3f((-hp.pelvisSize.x + hp.thighSize.x) / 2.f, 0, 0), vec3f());
	mpelvis.append(resource, mrthigh, vec3f((hp.pelvisSize.x - hp.thighSize.x) / 2.f, 0, 0), vec3f());

	//Append all the parts to the body ...
	body.append(resource, mpelvis, vec3f(0, hp.footSize.y + hp.lowerLegsSize.y + hp.thighSize.y, 0), vec3f()); // , vec3f(0, footSize.y + lowerLegsSize.y + thighSize.y, 0));

	return body;
}

void Pi3Chumanoid::attachModel(Pi3Cmodel &body)
{
	//Get direct pointers to the body parts
	//NOTE: pointers will be invalidated if models are moved (e.g. vector relocation)
	//      Either update pointers before animating or make body model static

	head = body.find("head");
	torso = body.find("torso");
	pelvis = body.find("pelvis");

	leftHand = body.find("leftHand");
	leftLowerArm = body.find("leftLowerArm");
	leftUpperArm = body.find("leftUpperArm");

	rightHand = body.find("rightHand");
	rightLowerArm = body.find("rightLowerArm");
	rightUpperArm = body.find("rightUpperArm");

	leftFoot = body.find("leftFoot");
	leftLowerLeg = body.find("leftLowerLeg");
	leftThigh = body.find("leftThigh");

	rightFoot = body.find("rightFoot");
	rightLowerLeg = body.find("rightLowerLeg");
	rightThigh = body.find("rightThigh");
}

void Pi3Chumanoid::walk(vec3f dir, float speed)
{
	//Pi3Cmatrix thighRot;
	//thighRot.SetRotateX(std::sin(walkPos));
	leftThigh->matrix.SetRotateXbit((std::sin(walkPos) - 0.3f)*0.5f);
	rightThigh->matrix.SetRotateXbit((std::sin(walkPos + PI) - 0.3f)*0.5f);
	leftLowerLeg->matrix.SetRotateXbit((std::sin(walkPos) + 0.9f)*0.4f);
	rightLowerLeg->matrix.SetRotateXbit((std::sin(walkPos + PI) + 0.9f)*0.4f);

	leftUpperArm->matrix.SetRotateXbit((std::sin(walkPos + PI) + 0.2f)*0.5f);
	rightUpperArm->matrix.SetRotateXbit((std::sin(walkPos) + 0.2f)*0.5f);
	leftLowerArm->matrix.SetRotateXbit((std::sin(walkPos + PI) - 0.8f)*0.5f);
	rightLowerArm->matrix.SetRotateXbit((std::sin(walkPos) - 0.8f)*0.5f);

	//thighRot.SetRotateX(std::cos(walkPos));
	//rightThigh->matrix = rightThigh->matrix * thighRot;
	walkPos += speed;
}
