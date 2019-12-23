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

class Modeller {
public:

	Modeller(Pi3Cresource *resource, Pi3Cwindow *window);
	~Modeller();

	enum Projection { PERSPECTIVE, ORTHOGRAPHIC };
	enum SceneLayout { TOPLEFT = 0, TOPRIGHT = 1, BOTTOMLEFT = 2, BOTTOMRIGHT = 3, FULL = 4 };
	enum EditMode { ED_SELECT, ED_ROTATE, ED_MOVE, ED_SCALE, ED_CREATE, ED_CONTOUR, ED_ROTATESCENE };
	enum ViewProject { VIEW_LEFT, VIEW_RIGHT, VIEW_TOP, VIEW_BOTTOM, VIEW_FRONT, VIEW_BACK, VIEW_FREE, VIEW_PERSPECTIVE };
	enum CreateTool { CT_CUBOID, CT_SPHERE, CT_CYLINDER, CT_CONE, CT_TCONE, CT_TUBE, CT_TORUS, CT_WEDGE, CT_EXTRUDE, CT_LATHE, CT_TEXT, CT_LIBSHAPE, CT_LANDSCAPE, CT_NONE };

	struct viewInfo {

		void setRotMatrix(const vec3f &rot) {
			rotInvertMatrix.setRotateXY(rot);
			rotInvertMatrix = rotInvertMatrix.inverse();
		}

		void setRot(const vec3f &rot) {
			setRotMatrix(rot);
			this->rot = rot;
		}

		vec3f viewCoords(const vec3f& pos)
		{
			setRotMatrix(rot);
			float iz = (projection == ORTHOGRAPHIC) ? 2.f / zoom : zoom / PSPVALUE;
			return rotInvertMatrix.transformRotateVec(pos*iz);
		}

		/* calculate orthographic position of mouse in scene .. */
		vec3f calcMouseXYZ(const int mx, const int my) {
			vec3f cpos((float)(-(mx - viewport.x - viewport.width / 2)), (float)(-(my - viewport.y - viewport.height / 2)), 0);
			return viewCoords(cpos) + pan;
		}

		float zoomFactor() {
			return (projection == ORTHOGRAPHIC) ? zoom / 10.f : -5.f;
		}

		ViewProject viewProject;
		Projection projection = PERSPECTIVE;
		Pi3Crecti viewport;
		Pi3Cmatrix rotInvertMatrix;
		vec3f pos{ 0,0,0 };
		vec3f rot{ 0,0,0 };
		vec3f pan{ 0,0,0 };
		float zoom = 100.f;
	};

	void setupGUI(loadOptions &opts);
	void setCreateTool(const CreateTool tool);
	void setEditMode(const EditMode mode);
	void handleKeys();
	void handleEvents();
	void handleIMGui();
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
	viewInfo setupView(const ViewProject view);
	bool isPerspective() { return views[currentView].projection == PERSPECTIVE;  }
	bool initialised() { return (resource != nullptr); }
	void createShape(const Pi3Cmesh& mesh, const vec3f& pos, const uint32_t colour = 0xffffffff);
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
	vec2f nearzfarz{ 1.f, 5000.f };
	bool gridlock = false;
	Pi3Cmodel *selGroup = nullptr;
	//uint32_t currentSel = 1;
	std::string currentLib;
	std::string currentModel;
	uint32_t currentColour = 0xffffff;
	Pi3Cmaterial outlines;

	bool keypress = false;
	bool dragbar = false;

	Pi3Ctouch touch;

	int32_t currentView = -1;
	int32_t currentSelView = -1;
	viewInfo views[5];
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
	
	void touchPerspectiveView(viewInfo &vi);
	void touchOrthoView(viewInfo &vi);
	void touchObject(Pi3Cmodel& sbox, Pi3Cmodel& mgiz);
	void setDragBar(bool on, SDL_Cursor * newCursor);

};
