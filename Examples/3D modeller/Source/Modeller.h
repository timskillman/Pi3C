#pragma once
#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cavatar.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cimgui.h"
#include "Pi3Crect.h"
#include "ModellerGUI.h"
#include "Pi3Cgizmos.h"
#include "Pi3Cediting.h"
#include "Pi3CfileOBJ.h"
#include "Pi3CviewInfo.h"

class Modeller {
public:

	Modeller(Pi3Cresource *resource, Pi3Cwindow *window);
	~Modeller();

	enum EditMode { ED_SELECT, ED_ROTATE, ED_MOVE, ED_SCALE, ED_CREATE, ED_CONTOUR, ED_ROTATESCENE };
	enum CreateTool { CT_CUBOID, CT_SPHERE, CT_CYLINDER, CT_CONE, CT_TCONE, CT_TUBE, CT_TORUS, CT_WEDGE, CT_EXTRUDE, CT_LATHE, CT_TEXT, CT_LIBSHAPE, CT_LANDSCAPE, CT_NONE };

	void setupGUI(loadOptions &opts);
	void setCreateTool(const CreateTool tool);
	void setEditMode(const EditMode mode);
	void handleKeys();
	void handleEvents(std::vector<uint32_t>& eventList);
	void handleIMGUI();
	void touchScene();
	//void tweens();
	void render();
	void renderScene(viewInfo &view);
	void animate();
	//void open();
	void init();
	void setCursor(SDL_Cursor *newCursor);
	void setDragBarH(bool on) { setDragBar(on, WECursor); };
	void setDragBarV(bool on) { setDragBar(on, NSCursor); };
	void setFullScene();

	//void saveScene(const std::string &file, Pi3Cmodel *models);
	void clearScene();
	void clearGizmos();
	viewInfo setupView(const viewInfo::ViewProject view);
	bool isPerspective() { return views[currentView].projection == viewInfo::PERSPECTIVE;  }
	bool initialised() { return (resource != nullptr); }
	void createShape(const Pi3Cmesh& mesh, const vec3f& pos, const uint32_t colour = 0xffffffff);
	void createLandscape(const vec3f pos, const uint32_t colour);
	void saveFile(const std::string& path, const std::string& filename) { std::string err; Pi3CfileOBJ::save(path, filename, &scene, false, nullptr, err); }

	//vec3f getRelativeMove(const vec3f& pos);

	void clearSelections();
	void selectAll();

	Pi3Cresource *resource = nullptr;
	Pi3Cwindow *window = nullptr;
	Pi3Cscene scene;

	//void loadModelLibrary(const std::string &path, const std::vector<std::string> &vals);
	//Pi3Cmodel loadScene(const std::string &file, vec3f &grid);
	//Pi3Cmodel createScene(const uint32_t width, const uint32_t depth, Pi3Cmodel *usemodel, const vec3f &grid);
	//Pi3Cmodel * findModel(const std::string &modelName);		//searches all sub-libs to find model
	//Pi3Cmodel * selectLib(const std::string &libName);


	Pi3Cavatar player;
	std::map<std::string, Pi3Cmodel> modelLibrary;
	std::vector<std::string> libnames;

	//Pi3Cmodel *modelsLib;

	vec4f lastCol { 0, 0, 0, 0 };
	EditMode editMode = ED_SELECT;
	CreateTool createTool = CT_CUBOID;
	std::string libShape;				//Create tool library shape file

	//bool modeChange = true;
	//float edTransition = 0;
	//vec3f editpos{ 0, 0.f, -300.f };
	//vec3f editrot{ .7f, .7f, 0 };
	vec3f grid;
	bool gridlock = false;
	Pi3Cmodel *selGroup = nullptr;
	//uint32_t currentSel = 1;
	std::string currentLib;
	std::string currentModel;
	uint32_t currentColour = 0xffffff;
	Pi3Cmaterial outlines;

	bool keypress = false;
	bool dragbar = false;
	bool panning = false;
	bool rotating = false;
	bool zooming = false;

	Pi3Ctouch touch;

	viewInfo views[5];
	int32_t currentView = -1;
	int32_t currentSelView = -1;
	int32_t fullview = -1;

	//float zoom = 1.f;
	//vec2f pan{ 0,0 };

	//scene model refs ..
	int32_t brushref = -1;
	int32_t sceneModelRef = -1;
	int32_t skybox = -1;
	int32_t gridRef = -1;
	int32_t selboxRef = -1;
	int32_t moveGizmoRef = -1;
	std::string selectedName;
	uint32_t shapeCounts[50] {};

	Pi3Cedit editUndo;		//used for editing scene with undo/redo support

	float sc = 0.5f;

	float cx = 0;
	float cy = 0;
	vec3f currentPos;
	//bool ctrlKey = false;
	//bool shiftKey = false;

	SDL_Cursor * handCursor = nullptr;
	SDL_Cursor * arrowCursor = nullptr;
	SDL_Cursor * moveCursor = nullptr;
	SDL_Cursor * crossCursor = nullptr;
	SDL_Cursor * textCursor = nullptr;
	SDL_Cursor * currentCursor = nullptr;
	SDL_Cursor * WECursor = nullptr;
	SDL_Cursor * NSCursor = nullptr;

private:
	MGui mgui;
	//Pi3Cbbox3d selbbox;

	void touchPerspectiveView(viewInfo &vi);
	void touchOrthoView(viewInfo &vi);
	void touchObject(Pi3Cmodel& selmodel);
	void setDragBar(bool on, SDL_Cursor * newCursor);
	void setSelectionBox();
	void moveSelections(const vec3f& vec);
	void setCurrentSelView(int32_t selview);
};