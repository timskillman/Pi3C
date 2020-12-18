#include "bullets.h"
#include "Pi3Cshapes.h"

void Bullets::init(Pi3Cresource* resource, Pi3Cscene* scene, const uint32_t _maxBullets, const uint32_t _bulletLife)
{
	maxBullets = _maxBullets;
	bulletLife = _bulletLife;
	Pi3Cmesh bulletmesh = Pi3Cshapes::cuboid(vec3f(0, 0, -50), vec3f(2, 2, 150));
	Pi3Cmodel bulletmodel;
	bulletmodel.create(resource, &bulletmesh, 1, 0xff00ffff);
	bulletmodel.name = "bullet";
	bulletmodel.material.illum = 0;
	bulletmodel.visible = false;

	life.resize(maxBullets);
	bulletRefs.resize(maxBullets);
	pos.resize(maxBullets);
	dir.resize(maxBullets);

	for (size_t i = 0; i < maxBullets; i++) {
		bulletRefs[i] = scene->append3D(bulletmodel);
		life[i] = bulletLife;
	}
}

void Bullets::fire(Pi3Cscene* scene, const vec3f& pos, const vec3f& rot, const float flyspeed) {
	for (size_t i = 0; i < maxBullets; i++) {
		if (life[i] == bulletLife) {
			Pi3Cmodel& bullet = scene->models[bulletRefs[i]];
			bullet.move(pos);
			bullet.visible = true;
			bullet.matrix.setRotateXY(rot); bullet.matrix = bullet.matrix.inverse();
			this->dir[i] = vec3f(sinf(rot.y) * -cosf(rot.x), -sinf(rot.x), -cosf(rot.y) * cosf(rot.x)) * (20.f + flyspeed);
			this->pos[i] = pos + this->dir[i];
			return;
		}
	}
}

void Bullets::animate(Pi3Cscene* scene, const float ticks)
{
	start = -1;
	for (size_t b = 0; b < maxBullets; b++) {
		if (scene->models[bulletRefs[b]].visible && life[b] >= 0) {
			if (start < 0) start = b;
			pos[b] += dir[b] * ticks;
			scene->models[bulletRefs[b]].matrix.move(pos[b]);
			life[b]-=ticks;
			if (life[b] < 0) {
				scene->models[bulletRefs[b]].visible = false;
				life[b] = bulletLife;
			}
			end = b;
		}
	}
}

bool Bullets::hitCheck(Pi3Cscene* scene, const vec3f targetpos, const float targetRadius)
{
	for (size_t b = start; b <= end && life[b] < bulletLife; b++) {
		if ((pos[b] - targetpos).length() < targetRadius) {
			life[b] = bulletLife;
			scene->models[bulletRefs[b]].visible = false;
			return true;
		}
	}
	return false;
}