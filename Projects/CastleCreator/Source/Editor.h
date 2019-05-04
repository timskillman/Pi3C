#pragma once
#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cavatar.h"
#include "Pi3Chumanoid.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cimgui.h"

class edmodel {
public:

	enum attachType { FREE, GRID, WALL };
	Pi3Cmodel model;
	attachType attach;
};

class Editor {
public:

	Editor(Pi3Cresource *resource, Pi3Cwindow *window);

	void setupGUI(loadOptions &opts);
	void handleKeys();
	void handleEvents();
	void handleIMGui();
	void touchScene();
	void tweens();
	void render();
	void animate();
	void open();
	void loadModels(const std::string &modelLibrary, const std::string &scenefile);
	void saveScene(const std::string &file, Pi3Cmodel *models);
	void newScene(const uint32_t ref);

	bool initialised() { return (resource != nullptr); }

private:

	void loadModelLibrary(const std::string &path, const std::vector<std::string> &vals);
	Pi3Cmodel loadScene(const std::string &file, vec3f &grid);
	Pi3Cmodel createScene(const uint32_t width, const uint32_t depth, Pi3Cmodel *usemodel, const vec3f &grid);
	Pi3Cmodel * findModel(const std::string &modelName);		//searches all sub-libs to find model
	Pi3Cmodel * selectLib(const std::string &libName);

	Pi3Cresource *resource = nullptr;
	Pi3Cwindow *window = nullptr;

	Pi3Cimgui gui;
	Pi3Cscene scene;
	Pi3Cavatar player;
	std::map<std::string, Pi3Cmodel> modelLibrary;
	std::vector<std::string> libnames;
	std::vector<std::string> guifonts;

	Pi3Cmodel *modelsLib;
	Pi3Chumanoid body;

	vec4f lastCol { 0, 0, 0, 0 };
	bool editMode = 0;
	bool modeChange = true;
	float edTransition = 0;
	vec3f editpos{ 0, 0.f, -300.f };
	vec3f editrot{ .7f, .7f, 0 };
	vec3f grid;
	vec2f nearzfarz{ 1.f, 5000.f };
	bool gridlock = false;
	Pi3Cmodel *selGroup = nullptr;
	uint32_t currentSel = 1;
	std::string currentLib;
	std::string currentModel;
	bool keypress = false;
	Pi3Ctouch touch;

	//scene model refs ..
	int32_t brushref = -1;
	int32_t sceneModelRef = -1;
	int32_t skybox = -1;

	vec3f currentPos;
	vec3f currentRot;
	vec3f current3rd;

	Pi3Cimgui::rectStyle bsMenu;
	Pi3Cimgui::rectStyle bsHeading;
	Pi3Cimgui::rectStyle bsCategories;
	Pi3Cimgui::rectStyle bsItems;

	float sc = 0.5f;
	vec2f butSize { 60.f, 150.f };
	vec2f butPos { 70.f, 70.f };
	float butStep = 70.f;
};
