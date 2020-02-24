#include "aliens.h"

void Aliens::init(Pi3Cresource* resource, Pi3Cscene* scene, const uint32_t landref, const uint32_t ashipRef, const uint32_t _maxAliens)
{
	maxAliens = _maxAliens;
	//Pi3Cmesh alienmesh = Pi3Cshapes::sphere(vec3f(0, 0, 0), 1.f);
	//Pi3Cmodel& alienmodel = scene->models[ashipRef];
	//alienmodel.create(resource, &alienmesh);
	//alienmodel.material.SetColDiffuse(0xff00ff00);
	//alienmodel.name = "alien";
	//alienmodel.visible = true;

	alienrefs.resize(maxAliens);
	landHeight.resize(maxAliens);
	dir.resize(maxAliens);
	actions.resize(maxAliens);
	exploding.resize(maxAliens);

	Pi3Cbbox3d lbox = scene->models[landref].bbox;
	float toph = lbox.max.y;

	for (size_t i = 0; i < maxAliens; i++) {
		vec3f pos = vec3f((float)(rand() % 25000 - 12500), toph + (float)(rand() % 2000), (float)(rand() % 25000 - 12500));

		float ht = 1e8f;  //start from way up high!
		ht = scene->models[landref].collideFloor(resource, nullptr, -pos, ht, true);
		if (ht < 1e8f) {
			//float sc = 20.f + (float)(rand() % 100);
			//alienmodel.matrix.SetScale(sc);
			Pi3Cmodel& alienmodel = scene->models[ashipRef]; //need fresh ref in case ptr is corrupted
			alienmodel.matrix.move(pos);
			landHeight[i] = pos.y - ht;
			
			alienrefs[i] = scene->append3D(alienmodel); //init only once - needs changing
			dir[i] = vec3f(0, -3.f, 0);
			exploding[i] = 20;
			actions[i] = AL_DESCENDING;
		}
		else {
			i--;
		}
	}
}

void Aliens::animate(Pi3Cresource* resource, Pi3Cscene* scene, const uint32_t landref, const float ticks)
{

	int arrivals = 0;
	for (size_t b = 0; b < maxAliens; b++) {
		Pi3Cmodel& alien = scene->models[alienrefs[b]];

		if (alien.visible) {
			switch (actions[b]) {
			case AL_DESCENDING:
				alien.matrix.Translate(dir[b] * ticks);
				if (alien.matrix.y() < landHeight[b]) {
					actions[b] = AL_LANDED;
					const vec3f& pos = alien.matrix.position();
					float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
					dir[b] = -vec3f(pos.x / dist, 0, pos.z / dist);
				}
				break;
			case AL_LANDED:
				{
					alien.matrix.Translate(dir[b] * ticks);
					const vec3f& pos = alien.matrix.position()+vec3f(0,-150.f,0);
					float ht = 1e8f;  //start from way up high!
					ht = scene->models[landref].collideFloor(resource, nullptr, -vec3f(pos.x,10000.f,pos.z) , ht, true);
					if (ht < 1e8f) alien.matrix.sety(10000.f - ht + 150.f);
					float dist = sqrt(pos.x * pos.x + pos.z * pos.z);
					if (dist < 20.f) {
						actions[b] = AL_DESTINATION;
						alien.material.SetColDiffuse(0xff0000ff);
					}
				}
				break;
			case AL_DESTINATION:
				arrivals++;
				//alien.matrix.SetScale((float)(rand() % (50 * arrivals) + 200 * arrivals) / 10.f);
				break;
			case AL_EXPLODING:
				alien.matrix.SetScale(alien.matrix.get()[0]*0.95f);
				exploding[b]--;
				if (exploding[b] < 1) {
					alien.visible = false;
					actions[b] = AL_DEAD;
				}
				break;
			};
		}
	}
}

void Aliens::hit(Pi3Cresource* resource, Pi3Cscene* scene, Bullets& bullets) {
	for (size_t a = 0; a < maxAliens; a++) {
		Pi3Cmodel& alien = scene->models[alienrefs[a]];
		if (alien.visible) {
			const vec3f& pos = alien.matrix.position();
			if (bullets.hitCheck(scene, pos, 100.f)) {
				actions[a] = AL_EXPLODING;
				alien.material.SetColDiffuse(0xff0000ff);
				return;
			}
		}
	}
}
