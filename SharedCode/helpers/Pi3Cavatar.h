#pragma once
#include "Pi3Cmatrix.h"
#include "Pi3Cscene.h"
#include <vector>
#include <map>

class Pi3Cavatar {
public:

	enum moveType { move_fly, move_walk, move_car, move_plane, move_helicopter, move_path };

	std::map<std::string, SDL_Scancode> keymap = {
	{"KEY_A",SDL_SCANCODE_A},{"KEY_B",SDL_SCANCODE_B},{"KEY_C",SDL_SCANCODE_C},{"KEY_D",SDL_SCANCODE_D},
	{"KEY_E",SDL_SCANCODE_E},{"KEY_F",SDL_SCANCODE_F},{"KEY_G",SDL_SCANCODE_G},{"KEY_H",SDL_SCANCODE_H},
	{"KEY_I",SDL_SCANCODE_I},{"KEY_J",SDL_SCANCODE_J},{"KEY_K",SDL_SCANCODE_K},{"KEY_L",SDL_SCANCODE_L},
	{"KEY_M",SDL_SCANCODE_M},{"KEY_N",SDL_SCANCODE_N},{"KEY_O",SDL_SCANCODE_O},{"KEY_P",SDL_SCANCODE_P},
	{"KEY_Q",SDL_SCANCODE_Q},{"KEY_R",SDL_SCANCODE_R},{"KEY_S",SDL_SCANCODE_S},{"KEY_T",SDL_SCANCODE_T},
	{"KEY_U",SDL_SCANCODE_U},{"KEY_V",SDL_SCANCODE_V},{"KEY_W",SDL_SCANCODE_W},{"KEY_X",SDL_SCANCODE_X},
	{"KEY_Y",SDL_SCANCODE_Y},{"KEY_Z",SDL_SCANCODE_Z},{"KEY_0",SDL_SCANCODE_0},{"KEY_1",SDL_SCANCODE_1},
	{"KEY_2",SDL_SCANCODE_2},{"KEY_3",SDL_SCANCODE_3},{"KEY_4",SDL_SCANCODE_4},{"KEY_5",SDL_SCANCODE_5},
	{"KEY_6",SDL_SCANCODE_6},{"KEY_7",SDL_SCANCODE_7},{"KEY_8",SDL_SCANCODE_8},{"KEY_9",SDL_SCANCODE_9},
	{"KEY_LSHIFT",SDL_SCANCODE_LSHIFT},{"KEY_RSHIFT",SDL_SCANCODE_RSHIFT},{"KEY_SPACE",SDL_SCANCODE_SPACE} };

	struct navkeys {
		std::string forward = "KEY_W";
		std::string back = "KEY_S";
		std::string left = "KEY_A";
		std::string right = "KEY_D";
		std::string up = "KEY_R";
		std::string down = "KEY_F";
		std::string fire1 = "KEY_RETURN";
		std::string jump = "KEY_SPACE";
		std::string run = "KEY_LSHIFT";
	};

	struct avatarParams {
		moveType movement = move_walk;		// Movement type (fly,walk,car,plane,helicopter,path)
		vec3f position;
		vec3f rotation;
		vec3f thirdPerson;
		vec3f size{ 1.f, 2.f, 1.f };
		float walkSpeed = 1.f;
		float runSpeed = 2.f;
		float fallSpeed = 5.f;
		float flyspeed = 0.f;
	};

	Pi3Cavatar() : walkSpeed(1.f), runSpeed(2.f), fallSpeed(5.f), size(1.f, 2.f, 1.f), movement(move_walk), faster(false), newmove(true), onground(false) {}
	Pi3Cavatar(const avatarParams &p) { init(p); }
	~Pi3Cavatar() {}

	void init(const avatarParams &p);
	void setTicks(float tick) { ticks = tick; }
	void setPosition(vec3f position) { pos = position; this->matrix.move(position); }
	void setSize(vec3f size) { this->size = size; }
	void setRotation(vec3f rot) { this->rot = rot; }
	vec3f getPosition() { return pos; }
	vec3f getRotation() { return rot; }
	vec3f getSize() { return size; }
	SDL_Scancode mapkey(const std::string &key);
	void doKeys();

	void updateAndCollide(const Pi3Cscene *scene, const float ticks);

	void forward() { move((faster) ? -runSpeed : -walkSpeed); moved = true; }
	void back() { move((faster) ? runSpeed : walkSpeed); moved = true; }
	void left() { sideways((faster) ? runSpeed : walkSpeed); moved = true; }
	void right() { sideways((faster) ? -runSpeed : -walkSpeed); moved = true; }
	void up();
	void down();
	void rotate(const vec3f &rot);
	void jump();
	void run(bool faster) { this->faster = faster; }

	navkeys keys;

	bool moved;
	float flyspeed = 1.f;
	float altitude = 0;

private:

	std::vector<Pi3Cmodel> avatarModels;
	Pi3Cmatrix matrix;
	vec3f size;
	std::vector<vec3f> eye;

	float walkSpeed;
	float runSpeed;
	float fallSpeed;
	
	moveType movement;
	bool faster;
	bool onground;

	vec3f rot;
	vec3f pos;
	vec3f thirdPerson;		//view position from avatar
	vec3f nextpos;

	float ticks = 1.f;

	//uint32_t lastTime;	//used for time based movement

	void move(float speed);
	void sideways(float speed);
	//float getTicks();
	bool newmove;

};