#pragma once
#include "Pi3Cmatrix.h"
#include "Pi3Cscene.h"
#include <vector>
#include <map>

class Pi3Cavatar {
public:

	enum moveType { move_fly, move_walk, move_car, move_plane, move_helicopter, move_path };
	std::map<char, SDL_Scancode> keys = { 
		{'A',SDL_SCANCODE_A},{'B',SDL_SCANCODE_B},{'C',SDL_SCANCODE_C},{'D',SDL_SCANCODE_D},
		{'E',SDL_SCANCODE_E},{'F',SDL_SCANCODE_F},{'G',SDL_SCANCODE_G},{'H',SDL_SCANCODE_H},
	    {'I',SDL_SCANCODE_I},{'J',SDL_SCANCODE_J},{'K',SDL_SCANCODE_K},{'L',SDL_SCANCODE_L},
		{'M',SDL_SCANCODE_M},{'N',SDL_SCANCODE_N},{'O',SDL_SCANCODE_O},{'P',SDL_SCANCODE_P},
	    {'Q',SDL_SCANCODE_Q},{'R',SDL_SCANCODE_R},{'S',SDL_SCANCODE_S},{'T',SDL_SCANCODE_T},
		{'U',SDL_SCANCODE_U},{'V',SDL_SCANCODE_V},{'W',SDL_SCANCODE_W},{'X',SDL_SCANCODE_X},
		{'Y',SDL_SCANCODE_Y},{'Z',SDL_SCANCODE_Z},{'0',SDL_SCANCODE_0},{'1',SDL_SCANCODE_1},
		{'2',SDL_SCANCODE_2},{'3',SDL_SCANCODE_3},{'4',SDL_SCANCODE_4},{'5',SDL_SCANCODE_5},
		{'6',SDL_SCANCODE_6},{'7',SDL_SCANCODE_7},{'8',SDL_SCANCODE_8},{'9',SDL_SCANCODE_9},
		{'L',SDL_SCANCODE_LSHIFT},{'A',SDL_SCANCODE_A},{'A',SDL_SCANCODE_A}, };

	struct avatarParams {
		moveType movement = move_walk;		// Movement type (fly,walk,car,plane,helicopter,path)
		vec3f position;
		vec3f rotation;
		vec3f thirdPerson;
		vec3f size{ 1.f, 2.f, 1.f };
		float walkSpeed = 1.f;
		float runSpeed = 2.f;
		float fallSpeed = 5.f;
	};

	Pi3Cavatar() : walkSpeed(1.f), runSpeed(2.f), fallSpeed(5.f), size(1.f, 2.f, 1.f), movement(move_walk), faster(false), newmove(true), onground(false) {}
	Pi3Cavatar(const avatarParams &p) { init(p); }
	~Pi3Cavatar() {}

	void init(const avatarParams &p);
	void setTicks(float tick) { ticks = tick; }
	void setPosition(vec3f position) { this->matrix.move(position); }
	void setSize(vec3f size) { this->size = size; }
	void setOrientation(vec3f rot) { this->rot = rot; }
	vec3f getPosition() { return pos; }
	vec3f getRotation() { return rot; }
	vec3f getSize() { return size; }
	void moveKeys(const SDL_Scancode k_forward, const SDL_Scancode k_back, const SDL_Scancode k_left, const SDL_Scancode k_right, const SDL_Scancode k_up, const SDL_Scancode k_down);

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

	bool moved;

private:

	Pi3Cmodel avatarModel;
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