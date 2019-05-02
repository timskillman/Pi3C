#pragma once
#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cavatar.h"
#include "Pi3Chumanoid.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cimgui.h"
#include "Pi3Crect.h"

class Modeller {
public:

	Modeller(Pi3Cresource *resource, Pi3Cwindow *window);

	enum Projection { PERSPECTIVE, ORTHOGRAPHIC };
	enum SceneLayout { TOPLEFT = 0, TOPRIGHT = 1, BOTTOMLEFT = 2, BOTTOMRIGHT = 3 };
	enum EditMode { EM_SELECT, EM_ROTATE, EM_MOVE, EM_SCALE, EM_CREATE, EM_CONTOUR };
	enum ViewProject { VIEW_LEFT, VIEW_RIGHT, VIEW_TOP, VIEW_BOTTOM, VIEW_FRONT, VIEW_BACK, VIEW_FREE, VIEW_PERSPECTIVE };
	enum CreateTool { CT_CUBOID, CT_SPHERE, CT_CYLINDER, CT_CONE, CT_TCONE, CT_TUBE, CT_TORUS, CT_WEDGE, CT_EXTRUDE, CT_LATHE, CT_TEXT, CT_LIBSHAPE };

	struct viewInfo {

		void setRot(vec3f rot) {
			rotInvertMatrix.rotateMatrixXY(rot);
			rotInvertMatrix = rotInvertMatrix.inverse();
			this->rot = rot;
		}

		ViewProject viewProject;
		Projection projection = PERSPECTIVE;
		Pi3Crecti viewport;
		Pi3Cmatrix rotInvertMatrix;
		vec3f pos{ 0,0,0 };
		vec3f rot{ 0,0,0 };
		vec3f pan{ 0,0,0 };
		float zoom = 1.f;
	};

	void setupGUI(loadOptions &opts);
	void setCreateTool(CreateTool tool);
	void handleKeys();
	void handleEvents();
	void handleIMGui();
	void touchScene();
	//void tweens();
	void render();
	void renderScene(const viewInfo &prpz);
	void animate();
	//void open();
	void init();
	//void saveScene(const std::string &file, Pi3Cmodel *models);
	void newScene(const uint32_t ref);
	viewInfo setupView(const ViewProject view);

	bool initialised() { return (resource != nullptr); }

private:

	//void loadModelLibrary(const std::string &path, const std::vector<std::string> &vals);
	//Pi3Cmodel loadScene(const std::string &file, vec3f &grid);
	//Pi3Cmodel createScene(const uint32_t width, const uint32_t depth, Pi3Cmodel *usemodel, const vec3f &grid);
	//Pi3Cmodel * findModel(const std::string &modelName);		//searches all sub-libs to find model
	//Pi3Cmodel * selectLib(const std::string &libName);

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
	EditMode editMode = EM_SELECT;
	CreateTool createTool = CT_CUBOID;
	std::string libShape;				//Create tool library shape file

	//bool modeChange = true;
	//float edTransition = 0;
	//vec3f editpos{ 0, 0.f, -300.f };
	//vec3f editrot{ .7f, .7f, 0 };
	vec3f grid;
	vec2f nearzfarz{ 1.f, 5000.f };
	bool gridlock = false;
	Pi3Cmodel *selGroup = nullptr;
	uint32_t currentSel = 1;
	std::string currentLib;
	std::string currentModel;
	uint32_t currentColour = 0xffffff;

	bool keypress = false;
	Pi3Ctouch touch;

	int32_t currentView = -1;
	viewInfo views[4];

	//float zoom = 1.f;
	//vec2f pan{ 0,0 };

	//scene model refs ..
	int32_t brushref = -1;
	int32_t sceneModelRef = -1;
	int32_t skybox = -1;

	//vec3f currentPos;
	//vec3f currentRot;
	//vec3f current3rd;

	Pi3Cimgui::rectStyle bsMenu;
	Pi3Cimgui::rectStyle bsHeading;
	Pi3Cimgui::rectStyle bsIcons;
	Pi3Cimgui::rectStyle bsButtons;
	Pi3Cimgui::rectStyle bsItems;

	float sc = 0.5f;

	int menuHeight = 20;
	int topbarHeight = 36;
	int botbarHeight = 40;
	int leftbarWidth = 50;
	int rightbarWidth = 20;

	float dragBarX = 0.5f;
	float dragBarY = 0.5f;
	float dragBarThickness = 8.f;
	bool draggingBarX = false;
	bool draggingBarY = false;
};
