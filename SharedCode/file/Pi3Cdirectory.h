#pragma once

#include "SDL_syswm.h"
#include <string>
#include <vector>

#ifdef __WINDOWS__
#include <Windows.h>
#else
#include <dirent.h>
#endif

namespace Pi3Cdirectory
{
	std::vector<std::string> readDirectory(const char * path);
};
