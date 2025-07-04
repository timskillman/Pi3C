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

	void SetupGUI(loadOptions &opts);
	void SetCreateTool(const CreateTool tool);
	void SetEditMode(const EditMode mode);
	void SetMousePosition(int x, int y);

	void HandleKeys();
	void HandleEvents(std::vector<uint32_t>& eventList);
	void HandleIMGUI();
	void TouchScene();
	//void tweens();
	void Render();
	void RenderScene(viewInfo &view);
	void Animate();
	//void open();
	void Init();
	void SetCursor(SDL_Cursor *newCursor);
	void setDragBarH(bool on) { SetDragBar(on, WECursor); };
	void setDragBarV(bool on) { SetDragBar(on, NSCursor); };
	void DropMan();
	void SetFullScreen();
	void SetFullScene();

	void SaveScene(const std::string& path, const std::string& filename, bool selected, Pi3Cscene& scene);
	void ClearScene();
	void ClearGizmos();
	bool isPerspective() { return views[currentView].projection == viewInfo::PERSPECTIVE;  }
	bool initialised() { return (resource != nullptr); }
	void CreateModel(const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId, const uint32_t colour = 0xffffffff, std::string txfile="");
	void CreateLandscape(const vec3f pos, const uint32_t colour);
	void SaveFileOBJ(const std::string& path, const std::string& filename, bool selected = false);
	bool LoadModelAtPosition(const std::string& modelfile, const vec3f pos, const std::function<void(float)> showProgressCB);
	bool currentViewIsActive() { return (currentView != viewInfo::INACTIVE && !mgui.somethingSelected()) || fullscreen; }

	void Snapshot();
	void ClearSelections();
	void SelectAll();
	void DeleteSelection();
	void DuplicateSelection();

	viewInfo SetupView(const viewInfo::ViewProject view);
	Pi3Crecti ViewRect(const viewInfo::SceneLayout projection);


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

	void CreateCursors();
	void DeleteCursors();

	void TouchView(viewInfo &vi);
	void TouchPerspectiveView(viewInfo &vi);
	void TouchOrthoView(viewInfo &vi);
	void TouchObject(Pi3Cmodel& selmodel);
	void SetDragBar(bool on, SDL_Cursor * newCursor);
	void SetSelectionBox();
	void SetSelectionRect();
	void MoveSelections(const vec3f& vec);
	void SetCurrentSelView(int32_t selview);
	void HandleKeyPresses();
	void CreateShapes();
	void CreatingShape();
	void CreateBlocks(const vec3f pos);
	void ClickLeftMouseButton(viewInfo& view);
	void ClickRightMouseButton(viewInfo& view);
	void MouseButtonUp();
	void DragLeftMouseButton(viewInfo& view, vec3f& mouseXYZ);
	void DragMiddleMouseButton(viewInfo& view, vec3f& mouseXYZ);
	void SetTouchFlags(bool val);
	void ResetZoom();
	void Navikeys(SDL_Scancode key, SDL_Scancode keyA, SDL_Scancode KeyB);
	void AddLinePoint(const vec3f point);
	void DeleteLinePoint();
	void UpdateLineIndexes();
	void FinishLine();
	void TransformLines(std::vector<vec3f>& lines, std::vector<vec2f>& contour, Pi3Cmatrix& matrix, int32_t start = 0);
	float GetShapeHeight(vec3f& pos, vec3f& v1, vec3f& v2);
	void RenderView(const viewInfo::SceneLayout projection, const Pi3Crecti& rect, int32_t mx, int32_t my);
	void ResetLineDrawing();
	void ShowLoadingBar(float perc);
	std::function<void(float)> SetCallback();

	Pi3Cmodel* brush() { return &scene.models[brushref]; }
	Pi3Cmodel* selRectangle() { return &scene.models2D[selRectRef]; }
};
