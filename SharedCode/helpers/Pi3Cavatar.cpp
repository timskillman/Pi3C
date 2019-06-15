#include "Pi3Cavatar.h"

void Pi3Cavatar::init(const avatarParams &p) 
{
	movement = p.movement;
	pos = p.position;
	rot = p.rotation;
	thirdPerson = p.thirdPerson;
	size = p.size;
	walkSpeed = p.walkSpeed;
	runSpeed = p.runSpeed;
	fallSpeed = p.fallSpeed;
	flyspeed = p.flyspeed;
	nextpos = pos;
	faster = false;
	newmove = true;
	moved = false;
	onground = false;
}

SDL_Scancode Pi3Cavatar::mapkey(const std::string &key)
{
	auto findkey = keymap.find(key);
	if (findkey != keymap.end()) return findkey->second;
	return SDL_SCANCODE_UNKNOWN;
}

void Pi3Cavatar::doKeys()
{
	const Uint8 * keystate = SDL_GetKeyboardState(NULL);
	if (keys.forward != "" && keys.forward[0] == 'K' && keystate[mapkey(keys.forward)]) forward();
	if (keys.back != "" && keys.back[0] == 'K' && keystate[mapkey(keys.back)]) back();
	if (keys.left != "" && keys.left[0] == 'K' && keystate[mapkey(keys.left)]) left();
	if (keys.right != "" && keys.right[0] == 'K' && keystate[mapkey(keys.right)]) right();
	if (keys.up != "" && keys.up[0] == 'K' && keystate[mapkey(keys.up)]) up();
	if (keys.down != "" && keys.down[0] == 'K' && keystate[mapkey(keys.down)]) down();
	if (keys.jump != "" && keys.jump[0] == 'K' && keystate[mapkey(keys.jump)]) jump();
	faster = (keys.run != "" && keys.run[0] == 'K' && keystate[mapkey(keys.run)]);
}

void Pi3Cavatar::move(const float speed)
{

	if (newmove) { nextpos = pos; newmove = false; }
	
	if (movement == move_fly)
		nextpos += vec3f(sinf(rot.y)*-cosf(rot.x), -sinf(rot.x), -cosf(rot.y)*cosf(rot.x)) * (speed *ticks);
	else
		nextpos += vec3f(-sinf(rot.y), 0, -cosf(rot.y)) * (speed *ticks);

	moved = true;
}

void Pi3Cavatar::sideways(const float speed)
{
	if (newmove) { nextpos = pos; newmove = false; }

	//if (movement == move_fly)
		//nextpos += vec3f(-cosf(rot.y)*-cosf(rot.x), -sinf(rot.x), sinf(rot.y)*cosf(rot.x)) * speed;
	//else
	nextpos += vec3f(cosf(rot.y), 0, -sinf(rot.y)) * (speed  * ticks);
	moved = true;
}

void Pi3Cavatar::up() 
{
	if (newmove) { nextpos = pos; newmove = false; }
	nextpos.y -= ticks;
	moved = true;
}

void Pi3Cavatar::down() 
{
	if (newmove) { nextpos = pos; newmove = false; }
	nextpos.y += ticks;
	moved = true;
}

void Pi3Cavatar::rotate(const vec3f &rot)
{
	this->rot += rot;
	moved = true;
}

void Pi3Cavatar::jump() 
{
	if (newmove) { nextpos = pos; newmove = false; }
	nextpos.y += 2.f;  //horrible hack
	moved = true;
}

void Pi3Cavatar::updateAndCollide(const Pi3Cscene *scene, const float ticks)
{
	const vec3f bdir = -(nextpos - pos)*3.f;
	const vec3f negpos = -nextpos;
	const vec3f footpos = negpos + vec3f(0, -1.0f, 0);
	const vec3f headpos = negpos + vec3f(0, 2.0f, 0);

	this->ticks = ticks/2.f;

	onground = false;

	if (scene != nullptr) {
		if (movement==move_fly || (!scene->collide(footpos, bdir, 3.f) )) pos = nextpos; else this->ticks = 1.f; //&& !scene->collide(headpos, bdir, 8.f)

		if (movement == move_fly) {
			altitude = scene->collideFloor(pos);
			//SDL_Log("Altitude = %f",floorHt);
		}

		if (movement != move_fly) {
			//When walking, simply project a line down and find out where the floor is (collideFloor returns a height value)
			float floorHt = scene->collideFloor(pos);

			if (floorHt == 1e8f) { 
				nextpos = pos; 
				return; //no geometry hit
			}

			if (floorHt < size.y) {
				pos.y -= (size.y - floorHt);
				onground = true;
			}
			else if (floorHt > size.y) {
				pos.y = (floorHt - fallSpeed < size.y) ? pos.y -= (size.y - floorHt) : pos.y += fallSpeed;
				nextpos = pos;
				moved = true;
			}
			//SDL_Log("Ground height = %f", floorHt);
		}
		else {
			move(-flyspeed);
		}
	}
	else {
		pos = nextpos;
	}
	newmove = true;
}