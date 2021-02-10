#pragma once
#include "Pi3Cvector.h"
#include "Pi3Cmatrix.h"

class Pi3Cquaternion {
public:

	Pi3Cquaternion() : x(0), y(0), z(0), w(0) {}
	Pi3Cquaternion(float x, float y, float z, float w) { this->x = x;  this->y = y; this->z = z; this->w = w; }

	float x;
	float y;
	float z;
	float w;

	void rotateAboutX(float a);
	void rotateAboutY(float a);
	void rotateAboutZ(float a);
	void rotateAboutAxis(float x, float y, float z, float a);
	//void quaternionX(Pi3Cmatrix* q, float x, float w);
	//void quaternionY(Pi3Cmatrix* q, float y, float w);
	//void quaternionZ(Pi3Cmatrix* q, float z, float w);

	void matrixToQuat(Pi3Cmatrix& mtx);
	Pi3Cmatrix toMatrix();
	void toMatrix16floats(float* mtx);
	void combineQuat(Pi3Cquaternion q);

private:
	void quaternionXm(Pi3Cmatrix& matx, float a);
	void quaternionYm(Pi3Cmatrix& matx, float a);
	void quaternionZm(Pi3Cmatrix& matx, float a);

};