#pragma once
#include <string>
#include <vector>
#include "Pi3Cvector.h"
#include "Pi3Cscene.h"
#include "Pi3Cmodel.h"

#define AUtoMetres 1000 //149597870700

class solarObject {
public:

	solarObject(std::string name, Pi3Cscene *scene, Pi3Cmesh mesh, std::string txtrfile, double distance, double radius, double rotperiod = 1, double orbperiod = 1) :
		name(name), distance(distance), radius(radius), rotperiod(rotperiod), orbperiod(orbperiod)
	{
		Pi3Cmodel * m = addMesh(name, scene->resource, mesh, txtrfile, (float)radius);
		m->material.SetAnimationTexture(-0.001f, 0);
	}

	std::string name;

	std::string txSurface;
	std::string txAtmosphere;
	std::string txRing;

	double distance;
	double radius;
	double rotperiod;
	double orbperiod;

	std::vector<solarObject> objects;  //orbiting objects
	Pi3Cmodel model;

	//Keplar elements
	double semimajorAxis[2];
	double eccentricity[2];
	double inclination[2];
	double meanLongitude[2];
	double longitudeOfPerihelion[2];
	double longitudeOfTheAscendingNode[2];
	vec3 pos;

	void setKeplars(double a1, double a2, double e1, double e2, double i1, double i2, double L1, double L2, double w1, double w2, double o1, double o2);
	void updatePositions(double tMillisFromJ2000);
	void ComputePositionAtDate(double tMillisFromJ2000);
	solarObject * add(solarObject object);
	void add(solarObject &object, double a1, double a2, double e1, double e2, double i1, double i2, double L1, double L2, double w1, double w2, double o1, double o2);
	Pi3Cmodel * addMesh(std::string name, Pi3Cresource *resource, Pi3Cmesh mesh, std::string txtrfile, float scale);
	void render(Pi3Cresource &resource, Pi3Cmatrix matrix, Pi3Cshader &shader, double maxZ, Pi3Cmaterial *materialOverride = nullptr);

private:

	bool keplars = false;

};

