#include "Pi3CsolarObject.h"
#include <math.h>

#define Deg2Rad 0.01745329252

void solarObject::ComputePositionAtDate(double tMillisFromJ2000)
{
	if (!keplars) return;

	double tCenturiesFromJ2000 = tMillisFromJ2000 / (1000 * 60 * 60 * 24 * 365.25 * 100);
	double a = semimajorAxis[0] + semimajorAxis[1] * tCenturiesFromJ2000;
	double e = eccentricity[0] + eccentricity[1] * tCenturiesFromJ2000;
	double i = inclination[0] + inclination[1] * tCenturiesFromJ2000;
	double L = std::remainder(std::remainder(meanLongitude[1] * tCenturiesFromJ2000, 360) + meanLongitude[0], 360);
	double p = longitudeOfPerihelion[0] + longitudeOfPerihelion[1] * tCenturiesFromJ2000;
	double W = longitudeOfTheAscendingNode[0] + longitudeOfTheAscendingNode[1] * tCenturiesFromJ2000;

	i = i * Deg2Rad;
	L = L * Deg2Rad;
	p = p * Deg2Rad;
	W = W * Deg2Rad;

	double M = L - p;
	double w = p - W;

	double E = M;
	double dE = (E - e * sin(E) - M) / (1 - e * cos(E));
	E -= dE;
	while (dE > 1e-6 || dE < -1e-6) {
		dE = (E - e * sin(E) - M) / (1 - e * cos(E));
		E -= dE;
	}

	// calc 2D coords of orbit
	double P = a * (cos(E) - e);
	double Q = a * sin(E) * sqrt(1 - pow(e, 2));

	// rotate by argument of periapsis
	double cosw = cos(w);
	double sinw = sin(w);
	double x = cosw * P - sinw * Q;
	double y = sinw * P + cosw * Q;

	// rotate by inclination
	pos.y = sin(i) * x;
	x = cos(i) * x;

	// rotate by longitude of ascending node
	cosw = cos(W);
	sinw = sin(W);
	pos.x = cosw * x - sinw * y;
	pos.z = sinw * x + cosw * y;

	pos.x *= AUtoMetres; //convert Astronomical Units (AU) to metres
	pos.y *= AUtoMetres;
	pos.z *= AUtoMetres;
}

void solarObject::setKeplars(double a1, double a2, double e1, double e2, double i1, double i2, double L1, double L2, double w1, double w2, double o1, double o2) {
	semimajorAxis[0] = a1; semimajorAxis[1] = a2;
	eccentricity[0] = e1; eccentricity[1] = e2;
	inclination[0] = i1; inclination[1] = i2;
	meanLongitude[0] = L1; meanLongitude[1] = L2;
	longitudeOfPerihelion[0] = w1;  longitudeOfPerihelion[1] = w2;
	longitudeOfTheAscendingNode[0] = o1; longitudeOfTheAscendingNode[1] = o2;
	keplars = true;
}

void solarObject::updatePositions(double tMillisFromJ2000) {
	if (keplars) {
		ComputePositionAtDate(tMillisFromJ2000);
		model.matrix.move(vec3f((float)pos.x, (float)pos.y, (float)pos.z));
	}

	for (auto &o : objects) {
		o.updatePositions(tMillisFromJ2000);
	}
}

solarObject * solarObject::add(solarObject object) {
	//object.model.matrix.SetScale(0.1f);	
	object.model.matrix.move(vec3f(100.f, 0, 0));
	//model.addToGroup(object.model);
	object.setKeplars(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	objects.push_back(object);
	return &objects.back();
}

void solarObject::add(solarObject &object, double a1, double a2, double e1, double e2, double i1, double i2, double L1, double L2, double w1, double w2, double o1, double o2)
{
	//model.addToGroup(object.model);
	object.setKeplars(a1, a2, e1, e2, i1, i2, L1, L2, w1, w2, o1, o2);
	objects.push_back(object);
}

Pi3Cmodel * solarObject::addMesh(std::string name, Pi3Cresource *resource, Pi3Cmesh mesh, std::string txtrfile, float scale)
{
	//Pi3Cmodel model(resource, mesh, 0xffffffff);
	model.create(resource, &mesh, 0xffffffff);
	model.name = name;
	float sc = scale / 500.f;
	if (sc < 100.f) sc = 100.f;
	if (sc > 1000.f) sc = 1000.f;
	model.matrix.SetScale(sc);
	if (txtrfile != "") model.addTexture(resource, txtrfile);
	//model.appendMesh(resource, mesh, false);
	//model.addToGroup(gmodel);
	
	return &model;
}

void solarObject::render(Pi3Cresource &resource, Pi3Cmatrix matrix, Pi3Cshader &shader, double maxZ, Pi3Cmaterial *materialOverride)
{
	Pi3Cmatrix newmatrix = matrix;
	double dist = pos.length();
	if (dist > maxZ) {
		//If object position is beyond the z-limit, 
		//then scale the object to look as if it's further away ...
		Pi3Cmatrix scmat;
		float sc = (float)(maxZ / (dist - maxZ));
		scmat.SetScale(sc);
		newmatrix = newmatrix * scmat;
	}
	//Pi3Cmatrix newmatrix = model.matrix * matrix;
	//model.render(resource, newmatrix, shader, materialOverride);
	for (auto &obj : objects) {
		//Pi3Cmatrix newmatrixg = model.matrix * newmatrix;
		//obj.render(resource, matrix, shader, maxZ, materialOverride);
	}
}

