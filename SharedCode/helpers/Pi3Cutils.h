#pragma once
#include "Pi3Cvector.h"
#include <vector>

namespace Pi3Cutils
{

	int32_t tesselatePath(std::vector<std::vector<float>>& path, std::vector<float>& tris, std::vector<std::vector<float>>& edges);
	
}
