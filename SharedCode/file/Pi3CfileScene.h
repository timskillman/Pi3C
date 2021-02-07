#pragma once
#include "Pi3Cjson.h"
#include "Pi3Cscene.h"

class Pi3CfileScene {
public:

	Pi3CfileScene();
	Pi3CfileScene(const std::string& path, const std::string& filename, bool selected, Pi3Cscene& scene) { save(path, filename, selected, scene); }

	void save(const std::string& path, const std::string& filename, bool selected, Pi3Cscene& scene);
	void loadSceneJSON(const std::string& file, Pi3Cscene& scene);
};