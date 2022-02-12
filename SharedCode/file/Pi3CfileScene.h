#pragma once
#include "Pi3Cjson.h"
#include "Pi3Cscene.h"

class Pi3CfileScene {

	Pi3CfileScene();

	void save(const std::string& filename, Pi3Cscene& scene);

};