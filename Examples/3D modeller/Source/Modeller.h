#pragma once
#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cavatar.h"
#include "Pi3CloadOptions.h"
//#include "Pi3CfileScene.h"
#include "Pi3Cimgui.h"
#include "Pi3Crect.h"
#include "ModellerGUI.h"
#include "Pi3Cgizmos.h"
#include "Pi3Cediting.h"
#include "Pi3CfileOBJ.h"
#include "Pi3CviewInfo.h"
#include "Pi3Cblocks.h"

class Modeller {
public:

	Modeller(Pi3Cresource *resource, Pi3Cwindow *window);
	~Modeller();

	enum EditMode { ED_SELECT, ED_ROTATE, ED_MOVE, ED_SCALE, ED_CREATE, ED_CONTOUR, ED_ROTATESCENE, ED_DROPMAN };
	enum CreateTool { CT_CUBOID, CT_SPHERE, CT_CYLINDER, CT_CONE, CT_TCONE, CT_TUBE, CT_TORUS, CT_WEDGE, CT_EXTRUDE, CT_LATHE, CT_TEXT, CT_LIBSHAPE, CT_LANDSCAPE, CT_LINE, CT_BLOCKS, CT_NONE };
	enum SceneAction { SA_NONE, SA_PANNING, SA_ZOOMING, SA_ROTATING, SA_DRAGBAR };

	void setupGUI(loadOptions &opts);
	void setCreateTool(const CreateTool tool);
	void setEditMode(const EditMode mode);
	void setMousePosition(int x, int y);

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
	void dropMan();
	void setFullScreen();
	void setFullScene();

	void saveScene(const std::string& path, const std::string& filename, bool selected, Pi3Cscene& scene);
	void clearScene();
	void clearGizmos();
	bool isPerspective() { return views[currentView].projection == viewInfo::PERSPECTIVE;  }
	bool initialised() { return (resource != nullptr); }
	void createModel(const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId, const uint32_t colour = 0xffffffff, std::string txfile="");
	void createLandscape(const vec3f pos, const uint32_t colour);
	void saveFile(const std::string& path, const std::string& filename, bool selected = false);
	bool loadModelAt(const std::string& modelfile, const vec3f pos, const std::function<void(float)> showProgressCB);
	bool currentViewIsActive() { return (currentView != viewInfo::INACTIVE && !mgui.somethingSelected()) || fullscreen; }

	void snapshot();
	void clearSelections();
	void selectAll();
	void deleteSelection();
	void duplicateSelection();

	viewInfo setupView(const viewInfo::ViewProject view);
	Pi3Crecti viewRect(const viewInfo::SceneLayout projection);


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
	SceneAction sceneAction = SceneAction::SA_NONE;

	int createCount = 0;
	int lastCreateCount = 0;
	int maxSteps = 1;
	Pi3Cmesh currentShape;
	vec3f createFirstPoint;
	vec3f oldPos;
	vec3f oldPos2;
	float oldht;
	std::vector<vec3f> lines; //used for creating lines/extrude and lathe
	std::vector<std::vector<vec3f>> linePaths;
	std::vector<std::vector<vec2f>> contours; //used for creating Extrude

	std::string libShape;				//Create tool library shape file
	int32_t modelGroupId = 1;			// used for identifying model materials

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
	bool fullscreen = false;

	bool selectRect = false;
	Pi3Crecti selRect;
	int32_t selRectRef;

	Pi3Ctouch touch;

	viewInfo views[6];
	int32_t currentView = viewInfo::BOTTOMRIGHT;
	int32_t currentSelView = currentView;
	int32_t fullview = viewInfo::INACTIVE;

	//scene model refs ..
	int32_t brushref = -1;
	int32_t sceneModelRef = -1;
	int32_t skybox = -1;
	int32_t gridRef = -1;
	int32_t selboxRef = -1;
	int32_t moveGizmoRef = -1;
	int32_t outlineRef = -1;
	std::string selectedName;
	uint32_t shapeCounts[50] {};

	Pi3Cedit editUndo;		//used for editing scene with undo/redo support

	float sc = 0.5f;

	float cx = 0;
	float cy = 0;
	vec3f currentPos;
	int32_t lineCount = 0;
	int32_t lastMovePoint = 0;
	vec3f lastPoint;
	//bool ctrlKey = false;
	//bool shiftKey = false;

	SDL_Cursor * handCursor = nullptr;
	SDL_Cursor * arrowCursor = nullptr;
	SDL_Cursor * moveCursor = nullptr;
	SDL_Cursor * crossCursor = nullptr;
	SDL_Cursor * textCursor = nullptr;
	SDL_Cursor * currentCursor = nullptr;
	SDL_Cursor* prevCursor = nullptr;
	SDL_Cursor * WECursor = nullptr;
	SDL_Cursor * NSCursor = nullptr;

private:
	MGui mgui;
	//Pi3Cbbox3d selbbox;

	std::string info;
	Blocks blockMap;

	void touchView(viewInfo &vi);
	void touchPerspectiveView(viewInfo &vi);
	void touchOrthoView(viewInfo &vi);
	void touchObject(Pi3Cmodel& selmodel);
	void setDragBar(bool on, SDL_Cursor * newCursor);
	void setSelectionBox();
	void setSelectionRect();
	void moveSelections(const vec3f& vec);
	void setCurrentSelView(int32_t selview);
	void handleKeyPresses();
	void createShapes();
	void creatingShape();
	void createBlocks(const vec3f pos);
	void ClickLeftMouseButton(viewInfo& view);
	void ClickRightMouseButton(viewInfo& view);
	void MouseButtonUp();
	void DragLeftMouseButton(viewInfo& view, vec3f& mouseXYZ);
	void DragMiddleMouseButton(viewInfo& view, vec3f& mouseXYZ);
	void setTouchFlags(bool val);
	void resetZoom();
	void navikeys(SDL_Scancode key, SDL_Scancode keyA, SDL_Scancode KeyB);
	void addLinePoint(const vec3f point);
	void delLinePoint();
	void updateLineIndexes();
	void finishLine();
	void transformLines(std::vector<vec3f>& lines, std::vector<vec2f>& contour, Pi3Cmatrix& matrix, int32_t start = 0);
	float getShapeHeight(vec3f& pos, vec3f& v1, vec3f& v2);
	void renderView(const viewInfo::SceneLayout projection, const Pi3Crecti& rect, int32_t mx, int32_t my);
	void resetLineDrawing();
	void loadingBar(float perc);
	std::function<void(float)> setCallback();

	Pi3Cmodel* brush() { return &scene.models[brushref]; }
	Pi3Cmodel* selRectangle() { return &scene.models2D[selRectRef]; }
};
