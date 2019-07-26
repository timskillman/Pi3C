#include "Pi3Ccontroller.h"

void Pi3Ccontroller::init() {

	if (SDL_NumJoysticks() < 1)
	{
		SDL_Log("Warning: No joysticks connected!\n");
	}
	else
	{
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == nullptr)
		{
			SDL_Log("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}

}

SDL_Scancode Pi3Ccontroller::mapkey(const std::string &key)
{
	auto findkey = keymap.find(key);
	if (findkey != keymap.end()) return findkey->second;
	return SDL_SCANCODE_UNKNOWN;
}
