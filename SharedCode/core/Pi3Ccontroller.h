#pragma once

#include "Pi3Cwin.h"
#include <vector>
#include <map>

class Pi3Ccontroller {
public:

	Pi3Ccontroller() { init(); }

	~Pi3Ccontroller() {
		SDL_JoystickClose(gGameController);
		gGameController = nullptr;
	}

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
		SDL_Scancode forward = SDL_SCANCODE_W;
		SDL_Scancode back = SDL_SCANCODE_S;
		SDL_Scancode left = SDL_SCANCODE_A;
		SDL_Scancode right = SDL_SCANCODE_D;
		SDL_Scancode up = SDL_SCANCODE_R;
		SDL_Scancode down = SDL_SCANCODE_F;
		SDL_Scancode fire1 = SDL_SCANCODE_RETURN;
		SDL_Scancode jump = SDL_SCANCODE_SPACE;
		SDL_Scancode run = SDL_SCANCODE_LSHIFT;
	};

	void init();
	SDL_Scancode mapkey(const std::string &key);

	SDL_Joystick* gGameController = nullptr;

};