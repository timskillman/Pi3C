#include "Pi3Ctouch.h"

void Pi3Ctouch::calcRay(vec3f touchPoint, float perspective)
{
	meshRef = -1;
	triRef = -1;
	touching = false;
	maxlevel = 0;
	selmodel = nullptr;
	//selgroup = nullptr;
	intersection = vec3f(0, 0, -1e8f);
	prevDist = 1e8f;
	this->perspective = perspective;
	this->touchPoint = touchPoint;

	float w1 = (perspective != 0) ? touchPoint.z / perspective : 1.f;
	float w2 = (perspective != 0) ? (touchPoint.z - 1.f) / perspective : 1.f;

	raypos = vec3f(touchPoint.x*w1, touchPoint.y*w1, touchPoint.z);
	raydir = vec3f(touchPoint.x*w2, touchPoint.y*w2, touchPoint.z - 1.f) - raypos; //ray is 'bent' by perspective value

	raydir.normalise();
}