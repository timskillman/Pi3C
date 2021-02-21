#include "Modeller.h"
#include "Pi3Cshapes.h"
#include "Pi3Cblocks.h"
#include "Pi3CfileFBX.h"
//#include "Pi3CfileGLTF.h"
#include "Pi3CfileFXGLTF.h"

#include <fstream>
#include <sstream>

using std::placeholders::_1;

Modeller::Modeller(Pi3Cresource *resource, Pi3Cwindow *window)
{
	// Setup shader ..
	int32_t basicShaderRef = resource->addShader("assets/shaders/vs1.glsl", "assets/shaders/fs1.glsl");
	if (basicShaderRef < 0) {
		window->showErrorMessage("Shader Error!", resource->error());
		return;
	}

	this->resource = resource;
	this->window = window;

	// Setup scene ...
	scene.init(resource);
	scene.selectShader(basicShaderRef);
	scene.setViewport2D(Pi3Crecti(0, 0, window->getWidth(), window->getHeight()),-200.f, 2000.f);
	window->setClearColour(Pi3Ccolours::DimGray);
	
	//scene.renderOffscreen(1200,800,resource->shaders[0]);
	
}

Modeller::~Modeller()
{
	if (handCursor) SDL_FreeCursor(handCursor);
	if (arrowCursor) SDL_FreeCursor(arrowCursor);
	if (moveCursor) SDL_FreeCursor(moveCursor);
	if (crossCursor) SDL_FreeCursor(crossCursor);
	if (textCursor) SDL_FreeCursor(textCursor);
	if (WECursor) SDL_FreeCursor(WECursor);
	if (NSCursor) SDL_FreeCursor(NSCursor);
}

void Modeller::clearScene()
{
	for (auto &model : scene.models) {
		if (model.touchable) model.deleted = true;
	}
	resource->deleteMaterialTexturesByID(modelGroupId);
	resource->deleteMaterialsByID(modelGroupId);
	resource->deleteVertsBuffer(modelGroupId);
	clearGizmos();
}

void Modeller::clearGizmos()
{
	touch.reset();
	scene.models[brushref].visible = false;
	scene.models[selboxRef].visible = false;
	scene.models[moveGizmoRef].visible = false;
	selectedName = "";
}

void Modeller::setupGUI(loadOptions &opts)
{

	mgui.init(opts, resource, window, "DefaultStyle.json");

	views[viewInfo::TOPLEFT] = setupView(viewInfo::VIEW_FRONT);
	views[viewInfo::TOPRIGHT] = setupView(viewInfo::VIEW_LEFT);
	views[viewInfo::BOTTOMLEFT] = setupView(viewInfo::VIEW_TOP);
	views[viewInfo::BOTTOMRIGHT] = setupView(viewInfo::VIEW_PERSPECTIVE);
	views[viewInfo::FULL] = setupView(viewInfo::VIEW_PERSPECTIVE);
	views[viewInfo::FULLSCREEN] = setupView(viewInfo::VIEW_PERSPECTIVE);
}

void Modeller::init()
{
	// Load 3D models and scene ...
	loadOptions opts("options.txt");

	setupGUI(opts);

	//nearzfarz = opts.asVec2f("nearzfarz");
	scene.setFog(0xffffff, 25000.f, 35000.f);
	//scene.setPerspective3D(window->getWidth(), window->getHeight(), PSPVALUE, nearzfarz.x, nearzfarz.y);

	// Setup player's avatar ...
	//Pi3Cavatar::avatarParams avparams;
	//avparams.movement = Pi3Cavatar::move_walk;
	//avparams.position = opts.asVec3f("startpos");
	//avparams.rotation = opts.asVec3f("rotation");
	//avparams.size = { 1.f, opts.asFloat("avatarHeight"), 1.f };
	//avparams.walkSpeed = opts.asFloat("avatarWalkSpeed");
	//avparams.runSpeed = opts.asFloat("avatarRunSpeed");
	//avparams.fallSpeed = opts.asFloat("avatarFallSpeed");
	//player.init(avparams);

	// Load Skybox ...
	std::string skyboxfile = opts.asString("skybox");
	if (skyboxfile != "") {
		skybox = scene.loadModelOBJ(opts.asString("skyboxPath"),skyboxfile , vec3f(), true); // false, loadbarCallback);
		scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
		scene.models[skybox].touchable = false;
	}

	// Create a brush for touching objects ...
	Pi3Cmodel brush = Pi3Cmodel(resource, Pi3Cshapes::sphere(vec3f(), 0.5f, 0.f, 10, 10),-1, 0xff00ffff);
	brush.touchable = false;
	brush.visible = false;
	brushref = scene.append3D(brush);

	// Create grid
	Pi3Cmodel gridModel = Pi3Cmodel(resource, Pi3Cshapes::grid(vec2f(100.f, 100.f), 10.f, 1.f, Pi3Ccolours::Graphite, Pi3Ccolours::Black),-1);
	gridModel.touchable = false;
	gridRef = scene.append3D(gridModel);

	// Create selection box
	Pi3Cmodel selboxModel = Pi3Cmodel(resource, Pi3Cgizmos::selectBoxGizmo(vec3f(), vec3f(), 0xffffff),-1);
	selboxModel.touchable = false;
	selboxRef = scene.append3D(selboxModel);

	Pi3Cmodel selRectModel = Pi3Cmodel(resource, Pi3Cshapes::rectLine(vec2f(0, 0), vec2f(0, 0)), -1);
	selRectModel.touchable = false;
	selRectModel.material.SetColAmbient(0xffffffff);
	selRectRef = scene.append2D(selRectModel);

	//Create outline shape
	Pi3Cmesh lmesh;
	for (uint32_t l = 0; l < 100000; l++) {
		lmesh.addPackedVert(vec3f(), vec3f(), vec2f(), 0xffffff);
	}
	lmesh.updateBounds();
	lmesh.materialRef = 0;
	lmesh.mode = GL_LINES;

	Pi3Cmodel outlineModel = Pi3Cmodel(resource, lmesh, 0xffffff);
	outlineModel.touchable = false;
	outlineModel.material.rendermode = GL_LINES;
	outlineRef = scene.append3D(outlineModel);

	// Create move gizmo
	Pi3Cmodel moveGizmo = Pi3Cmodel(resource, Pi3Cgizmos::moveGizmo(vec3f(), vec3f(), 0xffffffff),-1);
	moveGizmo.touchable = false;
	moveGizmo.visible = false;
	moveGizmo.material.rendermode = GL_LINES;
	moveGizmo.material.illum = 0;
	moveGizmoRef = scene.append3D(moveGizmo);

	outlines = *resource->defaultMaterial();
	outlines.rendermode = GL_LINE_STRIP; 
	outlines.illum = 1;

	handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	crossCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	moveCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	textCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	NSCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	WECursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	currentCursor = arrowCursor;
}

viewInfo Modeller::setupView(const viewInfo::ViewProject proj)
{
	const static float PIDIV2 = PI / 2.f;
	const static vec3f viewDir[8] = { { 0, PIDIV2, 0 }, { 0, -PIDIV2, 0 }, { PIDIV2, 0, 0 },{ -PIDIV2, 0, 0 },{}, { 0, PI, 0 },{ .7f, .7f, 0 },{ .7f, .7f, 0 } };

	viewInfo view;
	view.viewProject = proj;
	view.projection = (proj== viewInfo::VIEW_PERSPECTIVE) ? viewInfo::PERSPECTIVE : viewInfo::ORTHOGRAPHIC;
	view.setRot(viewDir[proj]);
	return view;
}

void Modeller::handleKeys()
{
	const uint8_t *keystate = window->getKeys();
}

void Modeller::resetZoom()
{
	viewInfo& view = views[currentView];
	view.pos = vec3f();
	if (view.zoom != 0) {
		view.pan += view.rotInvertMatrix.transformRotateVec(vec3f(0, 0, -view.zoom));
		view.zoom = 0;
	}
}

void Modeller::deleteSelection()
{
	editUndo.deleteSelection(scene.models);
	setTouchFlags(false);
	brush()->visible = false;
}

void Modeller::duplicateSelection()
{
	editUndo.duplicateSelection(scene.models);
	window->waitForMouseUp();
}

void Modeller::handleKeyPresses()
{
	SDL_Scancode kp = window->getKeyPress();

	if (window->ctrlKey) {
		switch (kp) {
		case SDL_SCANCODE_X:
			deleteSelection();
			break;
		case SDL_SCANCODE_Z:
			if (window->shiftKey)
				editUndo.redo(scene.models);
			else
				editUndo.undo(scene.models);
			break;
		case SDL_SCANCODE_Y:
			editUndo.redo(scene.models);
			break;
		case SDL_SCANCODE_A:
			selectAll();
			break;
		case SDL_SCANCODE_V:
			duplicateSelection();
			break;
		}
	}
	else {
		switch (kp) {
		case SDL_SCANCODE_DELETE:
			if (editMode == ED_CREATE && maxSteps == 0) {
				delLinePoint();
			}
			if (editMode == ED_SELECT) {
				editUndo.deleteSelection(scene.models);
				clearGizmos();
			}
			break;
		case SDL_SCANCODE_G:
			scene.models[gridRef].visible = !scene.models[gridRef].visible;
			window->waitForMouseUp();
			break;
		case SDL_SCANCODE_P:
			scene.renderOffscreen(views[currentSelView], &outlines);
			window->waitForMouseUp();
			break;
		case SDL_SCANCODE_T:
			scene.renderOffscreen(views[currentSelView], &outlines, 160, 128);
			window->waitForMouseUp();
			break;
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_S:
			navikeys(kp, SDL_SCANCODE_W, SDL_SCANCODE_S);
			break;
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_D:
			navikeys(kp, SDL_SCANCODE_A, SDL_SCANCODE_D);
			break;
		case SDL_SCANCODE_R:
		case SDL_SCANCODE_F:
			navikeys(kp, SDL_SCANCODE_F, SDL_SCANCODE_R);
			break;
		case SDL_SCANCODE_ESCAPE:
			if (fullscreen) 
				setFullScreen(); 
			else
				if (editMode == ED_CREATE && maxSteps == 0) resetLineDrawing();
			break;
		}
	}
	//keyPress = ev.key.keysym.scancode;
} 

void Modeller::navikeys(SDL_Scancode key, SDL_Scancode keyA, SDL_Scancode KeyB)
{
	if (currentViewIsActive()) {
		viewInfo& view = views[currentView];
		float speed = (window->shiftKey) ? 5.f : 0.5f;
		float dirspd = (key == keyA) ? speed : -speed;
		vec3f dir = (keyA == SDL_SCANCODE_F) ? vec3f(0, dirspd, 0) : (keyA == SDL_SCANCODE_A) ? vec3f(dirspd, 0, 0) : vec3f(0, 0, dirspd);
		resetZoom();
		view.pan += view.rotInvertMatrix.transformRotateVec(dir);
		setCurrentSelView(currentView);
	}
}

void Modeller::createBlocks(const vec3f pos)
{
	int mapSize = 10;
	blockMap.createBlocks(mapSize);
	
	for (int zb = 0; zb < mapSize; zb++) {
		for (int xb = 0; xb < mapSize; xb++) {
			blockMap.createMapChunk(xb, zb, 3);
		}
	}

	for (int zb = 0; zb < mapSize; zb++) {
		for (int xb = 0; xb < mapSize; xb++) {
			blockMap.createTrees(xb, zb, 5);
		}
	}

	int blocksId = modelGroupId;
	int texRef = resource->loadTexture("assets/maps/", "SurvivalCraft256x256.png", false);

	for (int zb = 1; zb < mapSize-1; zb++) {
		for (int xb = 1; xb < mapSize-1; xb++) {

			Pi3Cmesh blockmesh("ChunkMesh");
			blockMap.createMeshChunk(resource, blockmesh, xb, zb);

			createModel(blockmesh, pos, blocksId, 0xffffffff);
			Pi3Cmodel& model = scene.models.back();
			model.name = "ChunkModel";
			model.material.texRef = texRef;
			model.material.texID = resource->getTextureID(texRef);
			blocksId++; //TODO: Clear scene doesn't delete these block ID's ATM
		}
	}
}

std::function<void(float)> Modeller::setCallback() 
{
	mgui.takeSnapshot();
	scene.setup2D();
	return std::bind(&Modeller::loadingBar, this, _1);
}

void Modeller::createLandscape(const vec3f pos, const uint32_t colour)
{
	//scene.setup2D();
	//std::function<void(float)> loadbarCallback = setCallback();

	Pi3Ctexture maptex = Pi3Ctexture("assets/maps/mahe2.png", false);
	createModel(Pi3Cshapes::elevationMap(maptex, vec3f(0, -150.f, 0), vec3f(3000.f, 300.f, 3000.f), 764/2, 922 /2, 0,vec2f(1.f,1.f), setCallback()), pos, modelGroupId, colour);

	//createModel(Pi3Cshapes::readSRTM("assets/maps/S05E055.hgt"), pos, modelGroupId, colour);
}

void Modeller::createModel(const Pi3Cmesh& mesh, const vec3f& pos, int32_t groupId, const uint32_t colour, const std::string txfile)
{
	Pi3Cmodel shape = Pi3Cmodel(resource, mesh, groupId, colour);
	if (shape.meshRef >= 0) {
		clearSelections();
		//Give unique name for each shape ...
		shapeCounts[createTool]++;
		Pi3Cmesh* mesh = shape.getMesh(resource);
		vertsPtr vp = shape.getMeshVerts(resource);
		mesh->updateBounds(&vp);
		shape.name = mesh->name + std::to_string(shapeCounts[createTool]);
		shape.move(pos);
		shape.selected = true;
		int32_t modelRef = scene.append3D(shape, txfile);
		if (modelRef >= 0) editUndo.create(scene.models);
	}
}

uint32_t randomColour()
{
	uint32_t r = (rand() % 255);// +55;
	uint32_t g = (rand() % 255);// +55;
	uint32_t b = (rand() % 255);// +55;
	return r + (g << 8) + (b << 16);
}

void Modeller::createShapes()
{
	if (createTool != CT_NONE) {

		currentColour = randomColour();

		vec3f pos = -currentPos;
		createFirstPoint = pos;
		maxSteps = 0;

		switch (createTool) {
		case CT_CUBOID: createModel(Pi3Cshapes::cuboid(vec3f(), vec3f(.01f, .01f, .01f)), pos, modelGroupId, currentColour); maxSteps = 2; break;
		case CT_CYLINDER: createModel(Pi3Cshapes::cylinder(vec3f(), .01f, .01f), pos, modelGroupId, currentColour); maxSteps = 2; break;
		case CT_TUBE: createModel(Pi3Cshapes::tube(vec3f(), 0.005f, .01f, .01f), pos, modelGroupId, currentColour); maxSteps = 3; break;
		case CT_CONE: createModel(Pi3Cshapes::cone(vec3f(), .01f, .01f), pos, modelGroupId, currentColour); maxSteps = 2; break;
		case CT_TCONE: createModel(Pi3Cshapes::tcone(vec3f(), .01f, .01f, .01f), pos, modelGroupId, currentColour); maxSteps = 3; break;
		case CT_SPHERE: createModel(Pi3Cshapes::sphere(vec3f(), 0.05f), pos, modelGroupId, currentColour); maxSteps = 1; break;
		case CT_TORUS: createModel(Pi3Cshapes::torus(vec3f(), .01f, .01f), pos, modelGroupId, currentColour); maxSteps = 2; break;
		case CT_WEDGE: break;
		case CT_EXTRUDE: maxSteps = 0; break;
		case CT_LATHE: maxSteps = 0; break;
		case CT_LINE: maxSteps = 0; break;
		case CT_BLOCKS: createBlocks(pos); break;
		case CT_LANDSCAPE: createLandscape(pos, currentColour); break;
		}
	}
}

void Modeller::delLinePoint()
{
	if (lineCount <= lastMovePoint) return;

	Pi3Cmodel& model = scene.models[outlineRef];
	Pi3Cmesh* mesh = model.getMesh(resource);
	vertsPtr vp = model.getMeshVerts(resource);

	lines.pop_back();
	vp.offset = lines.size() * mesh->stride;
	mesh->vc = (lines.size() + 2) * mesh->stride;
	lineCount--;

	updateLineIndexes();
}

void Modeller::addLinePoint(const vec3f point)
{
	Pi3Cmodel& model = scene.models[outlineRef];
	Pi3Cmesh* mesh = model.getMesh(resource);
	vertsPtr vp = model.getMeshVerts(resource);
	
	float snapDistance = 3.f / views[currentView].zoomFactor();
	vec3f newPoint = (lineCount > (lastMovePoint + 1) && (point - createFirstPoint).length() < snapDistance) ? createFirstPoint : point;

	//work out mesh offsets and vert sizes for line contours ...
	vp.offset = lines.size() * mesh->stride;
	mesh->vc = (lines.size() + 2) * mesh->stride;

	//vec3f cp;
	//if (lines.size() > 1) {
	//	cp = (lines[lineCount - 1] ^ lines[(lineCount) % lines.size()]); // .unit() * 10.f;
	//}

	if (createCount != lastCreateCount) {

		//create new point (left mouse button) ...

		if (lineCount == lastMovePoint) {
			createFirstPoint = point;
			model.visible = true;
		}
		lines.push_back(newPoint);

		switch (createTool) {
		case CT_EXTRUDE:
			//Join points? 
			if (lineCount > (lastMovePoint+1) && newPoint == createFirstPoint) {
				linePaths.emplace_back();
				std::vector<vec3f>& pline = linePaths.back();
				for (int32_t i = lastMovePoint + 1; i < lines.size(); i++) {
					pline.push_back(lines[i]);
				}
				lastMovePoint = lineCount+1;
			}
			break;
		case CT_LATHE:
			break;
		}

		lineCount++;
		lastCreateCount = createCount;
	}

	Pi3Cshapes::addPoint(vp, newPoint);
	Pi3Cshapes::addPoint(vp, newPoint);

	updateLineIndexes();
}

void Modeller::updateLineIndexes()
{
	Pi3Cmodel& model = scene.models[outlineRef];
	Pi3Cmesh& mesh = *(model.getMesh(resource));

	mesh.lineIndexes.clear();
	int sc = 0;
	for (auto& c : linePaths) {
		for (int i = 0; i < c.size(); i++) {
			mesh.lineIndexes.push_back(sc + i);
			mesh.lineIndexes.push_back(sc + (i + 1) % c.size());
		}
		sc += c.size() + 1;
	}

	int c = lineCount - lastMovePoint;
	for (int i = 0; i < c; i++) {
		mesh.lineIndexes.push_back(sc + i);
		mesh.lineIndexes.push_back(sc + i + 1);
	}

	resource->updateMesh(model.meshRef);
}

void Modeller::transformLines(std::vector<vec3f>& lines, std::vector<vec2f>& contour, Pi3Cmatrix& matrix, int32_t start)
{
	for (int32_t i=start; i<lines.size(); i++)
	{
		vec3f vr = matrix.transformVec(lines[i]);
		contour.push_back(vec2f(vr.x, vr.y));
	}
}

void Modeller::finishLine()
{

	if (lines.size() > 2) {

		switch (createTool) {
		case CT_EXTRUDE:
			if (lineCount > (lastMovePoint + 1)) {
				//lines.push_back(createFirstPoint); //close current contour
				linePaths.push_back(lines);
				//contours.emplace_back();
				//std::vector<vec2f>& contour = contours.back();
				//transformLines(lines, contour, views[currentView].rotInvertMatrix, lastMovePoint + 1);
			}

			//int lm = 0;
			for (auto& lp : linePaths) {
				contours.emplace_back();
				std::vector<vec2f>& contour = contours.back();
				transformLines(lp, contour, views[currentView].rotInvertMatrix, 0);
				//lm = lp.size();
			}

			createModel(Pi3Cshapes::extrude("Extrude", vec3f(), contours, 1.f, 1), vec3f(), modelGroupId, currentColour);
			scene.lastModel()->transformVerts(resource, views[currentView].rotMatrix);
			break;
		case CT_LATHE:
			{
				std::vector<vec2f> contour;
				Pi3Cmatrix matrix;
				transformLines(lines, contour, views[currentView].rotInvertMatrix);
				//Rotate around first point ...
				vec2f fp = contour[0];
				for (auto& p : contour) {
					p.x -= fp.x;
				}
				createModel(Pi3Cshapes::lathe("Lathe", vec3f(fp.x, 0, 0.f), contour, true), vec3f(0, 0, 0.f), modelGroupId, currentColour);
				scene.lastModel()->transformVerts(resource, views[currentView].rotMatrix);
			}
			break;
		case CT_LINE:
			break;
		}
		
	}
	resetLineDrawing();
}

void Modeller::resetLineDrawing()
{
	createCount = 0;
	lineCount = 0;
	lastMovePoint = 0;
	maxSteps = 1;
	contours.clear();
	lines.clear();
	linePaths.clear();
	scene.models[outlineRef].visible = false;
}

float Modeller::getShapeHeight(vec3f& pos, vec3f& v1, vec3f& v2)
{
	float ht = 0;
	if (abs(oldPos.x - createFirstPoint.x) < 0.001f) {
		ht = -(pos.y - oldPos.y);
		pos.x = ht; pos.y = oldPos.y; pos.z = oldPos.z;
	}
	if (abs(oldPos.y - createFirstPoint.y) < 0.001f) {
		ht = -(pos.z - oldPos.z);
		pos.y = ht; pos.x = oldPos.x; pos.z = oldPos.z;
	}
	if (abs(oldPos.z - createFirstPoint.z) < 0.001f) {
		ht = -(pos.y - oldPos.y);
		pos.z = ht; pos.y = oldPos.y; pos.x = oldPos.x;
	}

	v1 = pos - createFirstPoint;
	v2 = vec3f(0, 0, 0);
	if (pos.x < createFirstPoint.x) { v2.x = v1.x; v1.x = -v1.x; }
	if (pos.y < createFirstPoint.y) { v2.y = v1.y; v1.y = -v1.y; }
	if (pos.z < createFirstPoint.z) { v2.z = v1.z; v1.z = -v1.z; }

	return ht;
}

void Modeller::creatingShape()
{
	vec3f pos = -currentPos;

	//if (selectRect) {
	//	selRect.width = window->mouse.x - selRect.x;
	//	selRect.height = (window->getHeight()-window->mouse.y) - selRect.y;
	//	setSelectionRect();
	//	SDL_Log("Sel rect: %d,%d,%d,%d", selRect.x, selRect.y, selRect.width, selRect.height);
	//	return;
	//}

	if (maxSteps == 0) {
		switch (createTool) {
		case CT_EXTRUDE:
		case CT_LATHE:
		case CT_LINE:
			addLinePoint(pos);
			return;
		default:
			return;
		}
	}

	Pi3Cmodel* currentModel = scene.lastModel();
	vertsPtr vp = currentModel->getMeshVerts(resource);
	vec3f v1, v2, csize;

	if (createCount == 1) {
		float dist = (pos - createFirstPoint).length();
		switch (createTool) {
		case CT_CUBOID:
			csize = pos - createFirstPoint;
			info = "Cuboid x:" + Pi3Cutils::ftostrdp(csize.x, 2) + " y:" + Pi3Cutils::ftostrdp(csize.y, 2) + " z:" + Pi3Cutils::ftostrdp(csize.z, 2);
			Pi3Cshapes::cube_verts(*vp.verts, vp.offset, (pos - createFirstPoint) / 2.f, csize, 1, 1, 1, 0xffffffff);
			break;
		case CT_CYLINDER:
			info = "Cylinder radius:" + Pi3Cutils::ftostrdp(dist, 2);
			Pi3Cshapes::cylinder_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist, 0.1f); 
			break;
		case CT_SPHERE:
			info = "Sphere radius:" + Pi3Cutils::ftostrdp(dist, 2);
			Pi3Cshapes::sphere_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist); 
			break;
		case CT_TORUS:
			info = "Torus radius:" + Pi3Cutils::ftostrdp(dist, 2);
			Pi3Cshapes::torus_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist, 0.01f); 
			break;
		case CT_CONE:
			info = "Cone radius:" + Pi3Cutils::ftostrdp(dist, 2);
			Pi3Cshapes::cone_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist, 0.01f); 
			break;
		case CT_TCONE:
			info = "Truncated cone bottom radius:" + Pi3Cutils::ftostrdp(dist, 2);
			Pi3Cshapes::tcone_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist, dist, 0.01f); 
			break;
		case CT_TUBE:
			info = "Tube inner radius:" + Pi3Cutils::ftostrdp(dist, 2);
			Pi3Cshapes::tube_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist, dist *1.01f, 0.01f); 
			break;
		}
		oldPos = pos;
		oldPos2 = pos;
		if (createTool != CT_CUBOID) currentModel->transformVerts(resource, views[currentView].rotInvertMatrix);
	}
	else if (createCount == 2) {
		float dist1 = (oldPos - createFirstPoint).length();
		float dist2 = views[currentView].rotInvertMatrix.transformVec(pos - oldPos).y;
		float ht = getShapeHeight(pos, v1, v2);
		switch (createTool) {
		case CT_CUBOID:
			{
				info = "Cuboid height:" + Pi3Cutils::ftostrdp(ht, 2);
				Pi3Cshapes::cube_verts(*vp.verts, vp.offset, v1 / 2.f + v2, v1, 1, 1, 1, 0xffffffff);
			}
			break;
		case CT_CYLINDER:
			{
				info = "Cylinder height:" + Pi3Cutils::ftostrdp(ht, 2);
				Pi3Cshapes::cylinder_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist1, ht); break;
			}
			break;
		case CT_CONE:
			{
				info = "Cone height:" + Pi3Cutils::ftostrdp(ht, 2);
				Pi3Cshapes::cone_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist1, ht); break;
			}
			break;
		case CT_TORUS:
			info = "Torus thickness:" + Pi3Cutils::ftostrdp(dist2*2.f, 2);
			Pi3Cshapes::torus_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist1, dist2); 
			break;
		case CT_TCONE:
			info = "Truncated cone top radius:" + Pi3Cutils::ftostrdp(ht, 2);
			Pi3Cshapes::tcone_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist1, dist1, ht); // (oldPos2 - createFirstPoint).length(), .1f);
			break;
		case CT_TUBE:
			{
				float r1 = dist1;
				float r2 = dist1+ht; // (oldPos2 - createFirstPoint).length();
				//if (r1 > r2) std::swap(r1, r2);
				info = "Tube outer radius:" + Pi3Cutils::ftostrdp(r2, 2);
				Pi3Cshapes::tube_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), r1, r2, .1f);
			}
			break;
		}
		if (createTool != CT_CUBOID) currentModel->transformVerts(resource, views[currentView].rotInvertMatrix);
		oldPos2 = pos;
		oldht = ht;
	}
	else if (createCount == 3) {
		float dist1 = (oldPos - createFirstPoint).length();
		//float dist2 = (oldPos2 - createFirstPoint).length();
		//float ht = (pos - oldPos2).length();
		float ht = getShapeHeight(pos, v1, v2);

		switch (createTool) {
		case CT_TCONE:
			{
				info = "Truncated cone height:" + Pi3Cutils::ftostrdp(ht, 2);
				Pi3Cshapes::tcone_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist1, dist1 + ht-oldht, oldht);
			}
			break;
		case CT_TUBE:
			{
				//if (dist1 > dist2) std::swap(dist1, dist2);
				info = "Tube height:" + Pi3Cutils::ftostrdp(ht, 2);
				Pi3Cshapes::tube_verts(*vp.verts, vp.offset, vec3f(0, 0, 0), dist1, dist1 + oldht, ht - oldht);
			}
			break;
		}
		currentModel->transformVerts(resource, views[currentView].rotInvertMatrix);
	}
	currentModel->refreshMesh(resource);
	//vp = resource->getMeshVerts(scene.lastModel().meshRef);
	//mesh.updateBounds(&vp);
	//model.bbox = mesh.bbox;
	////mesh.bbox.bboxFromVerts(*vp.verts, vp.offset, mesh.vc, mesh.stride);
	//resource->updateMesh(model.meshRef);

	//if (nextStep) createCount++;
}

void Modeller::MouseButtonUp()
{
	sceneAction = SA_NONE;
}

void Modeller::DragMiddleMouseButton(viewInfo& view, vec3f& mouseXYZ)
{
	setCurrentSelView(currentView);
	view.pan += view.viewCoords(mouseXYZ);
	sceneAction = SA_PANNING;
}

void Modeller::DragLeftMouseButton(viewInfo& view, vec3f& mouseXYZ)
{
	switch (editMode) {
	case ED_SELECT:
		if (selectRect) {
			selRect.width = window->mouse.x - selRect.x;
			selRect.height = (window->getHeight() - window->mouse.y) - selRect.y;
			setSelectionRect();
			if (selRect.width != 0 || selRect.height != 0) scene.models2D[selRectRef].visible = true;
			//SDL_Log("Sel rect: %d,%d,%d,%d", selRect.x, selRect.y, selRect.width, selRect.height);
		}
		break;
	case ED_CREATE:
		break;
	case ED_MOVE:
		moveSelections(view.viewCoords(mouseXYZ));
		setSelectionBox();
		break;
	case ED_ROTATE:
		break;
	case ED_SCALE:
		{
			float sc = -window->mouse.deltaXY.y * 0.1f;
			editUndo.scaleSelections(scene.models, vec3f(sc, sc, sc)); //view.viewCoords(mouseXYZ)*0.5f
			setSelectionBox();
		}
		break;
	case ED_ROTATESCENE:
		if (view.projection == viewInfo::PERSPECTIVE) {
			view.addRot(vec3f(mouseXYZ.y, mouseXYZ.x, 0) * -0.01f);
			//view.setRotMatrix(view.rot);
			sceneAction = SA_ROTATING;
			//rotating = true;
		}
		break;
	}
}

void Modeller::ClickLeftMouseButton(viewInfo& view)
{
	if ((currentViewIsActive() && window->mouse.LeftButton && sceneAction != SA_DRAGBAR)) {

		setCurrentSelView(currentView);

		switch (editMode) {
		case ED_CREATE:
			if (createCount == maxSteps) {
				info = "";
				createCount = 0;
			}
			else {
				if (createCount==0) createShapes();
				createCount++;
				creatingShape();
			}
			
			window->waitForMouseUp();
			break;
		case ED_SELECT:
			if (!window->ctrlKey) clearSelections();
			touchScene();
			if (touch.selmodel) {
				brush()->matrix.move(touch.intersection);
				editUndo.selectModel(scene.models, &scene.models[touch.parent()], window->ctrlKey);
			}
			else
			{
				selectRect = true;
				brush()->visible = false;
				selRect.x = window->mouse.x;
				selRect.y = window->getHeight() - window->mouse.y;
			}
				//clearSelections();
			break;
		case ED_MOVE:
			//if (!window->ctrlKey) clearSelections();
			touchScene();
			if (touch.selmodel) {
				editUndo.selectModel(scene.models, touch.selmodel, window->ctrlKey);
				scene.models[touch.parent()].selected = true;
			}
			else clearSelections();
			break;
		case ED_DROPMAN:
			touchScene();
			break;
		case ED_ROTATE:
		case ED_ROTATESCENE:
			setCursor(handCursor);
			break;
		case ED_SCALE:
			editUndo.setSelectionCentre(scene.models);
			//scene.models[selboxRef].visible = false;
			break;
		}

	}
}

void Modeller::ClickRightMouseButton(viewInfo& view)
{
	if (currentViewIsActive() && window->mouse.RightButton) {

		setCurrentSelView(currentView);

		switch (editMode) {
		case ED_CREATE:
			if (maxSteps == 0) {
				finishLine();
			}
			window->waitForMouseUp();
			break;
		case ED_SELECT:
			touchScene();
			if (touch.selmodel) {
				brush()->matrix.move(touch.intersection);
				editUndo.selectModel(scene.models, &scene.models[touch.parent()], window->ctrlKey);
				//scene.models[touch.groupRefs[0]].selected = touch.selmodel->selected;
			}
			else
				clearSelections();
			break;
		case ED_MOVE:
			touchScene();
			if (touch.selmodel) {
				editUndo.selectModel(scene.models, touch.selmodel, window->ctrlKey);
				scene.models[touch.parent()].selected = true;
			}
			else clearSelections();
			break;
		case ED_DROPMAN:
			touchScene();
			break;
		case ED_ROTATE:
			break;
		case ED_SCALE:
			editUndo.setSelectionCentre(scene.models);
			//scene.models[selboxRef].visible = false;
			break;
		}

	}
}

void Modeller::touchScene()
{
	if (sceneAction != SA_NONE) return;

	if (currentViewIsActive()) {

		touchView(views[currentView]);

		if (editMode == ED_CREATE) setCursor(crossCursor);

		//Pi3Cmodel& brush = scene.models[brushref];
		//Pi3Cmodel& selmodel = scene.models[touch.parent()];

		if (touch.selmodel) {

			//move 3D pointer (sphere) to touch intersetion point ...
			currentPos = -touch.intersection;

			//brush()->matrix.move(touch.intersection);
			setTouchFlags(true);
			touch.selmodel->selected = true;
			brush()->visible = false;

			switch (editMode) {
			case ED_SELECT: 
				brush()->visible = true;
				touchObject(*touch.selmodel);
				setCursor(arrowCursor);
				break;
			case ED_MOVE: 
				setCursor(moveCursor); 
				break;
			case ED_ROTATE: 
				setCursor(handCursor); 
				break;
			case ED_SCALE: 
				setCursor(handCursor); 
				break;
			case ED_DROPMAN: 
				dropMan(); 
				break;
			}

		}
		else {
			setTouchFlags(false);
			selectedName = "";
		}
	}
	else {
		setCursor(arrowCursor);
	}
}

void Modeller::loadingBar(float perc)
{
	mgui.showProgressCB(window, "Loading ", perc);
}

void Modeller::handleEvents(std::vector<uint32_t>& eventList)
{
	if (eventList.size() == 0) return;

	// process window events ...
	viewInfo& view = views[currentView];
	
	for (auto& ev : eventList) {
		switch (ev)
		{
		case SDL_MOUSEBUTTONDOWN:
			//SDL_Log("MouseDown");
			if (window->mouse.LeftButton) ClickLeftMouseButton(view);
			if (window->mouse.RightButton) ClickRightMouseButton(view);
			break;
		case SDL_MOUSEBUTTONUP:
			SDL_Log("MouseUp");
			if (selectRect) {
				//vec2f p1 = views[currentView].getMouseToWorldCoords(selRect.x, selRect.y);
				//vec2f p2 = views[currentView].getMouseToWorldCoords(selRect.x + selRect.width, selRect.y + selRect.height);
				//scene.touchRect(Pi3Crect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y), views[currentView]);
				scene.touchRect(Pi3Crect(selRect.x, selRect.y, selRect.width, selRect.height), views[currentView]);
				selRectangle()->visible = false;
				selectRect = false;
			}
			MouseButtonUp();
			break;
		case SDL_MOUSEMOTION:
			//SDL_Log("MouseMove");
			if (currentViewIsActive() && sceneAction != SA_DRAGBAR) {
				if (window->mouse.anyButton()) {
					vec3f mouseXYZ = vec3f(window->mouse.deltaXY.x, -window->mouse.deltaXY.y, 0);
					//float sc = -window->mouse.deltaXY.y * 0.1f;
					if (window->mouse.MiddleButton) DragMiddleMouseButton(view, mouseXYZ);
					if (window->mouse.LeftButton) DragLeftMouseButton(view, mouseXYZ);
				} else if (createCount > 0) {
					creatingShape();
					window->waitForMouseUp();
					break;
				}
			}
			break;
		case SDL_MOUSEWHEEL:
			if (currentViewIsActive()) {
				view.zoom += window->mouse.wheel * view.zoomFactor();
				setCurrentSelView(currentView);
				sceneAction = SA_ZOOMING;
			}
			break;
		case SDL_WINDOWEVENT:
			if (window->resized) {
				mgui.resize();
				window->resized = false;
			}
			break;
		case SDL_KEYDOWN:
			handleKeyPresses();
			break;
		case SDL_DROPFILE: 
			std::string file = window->dropfile;
			std::string ext = Pi3Cutils::getExt(file); // .substr(file.size() - 4, 4);

			if (loadModelAt(file, touch.touching ? touch.intersection : vec3f(), setCallback())) { }

			else if (ext == "png" || ext == "jpg") {
				setCurrentSelView(currentView);
				setMousePosition(window->mouse.x, window->getHeight() - window->mouse.y);
				touchView(views[currentView]);
				if (touch.touching) {
					touch.selmodel->addTexture(resource, file.c_str());
					touch.selmodel->material.SetColDiffuse(0xffffffff);
				}
			}
			mgui.startSnapshot();
			break;
		}
	}

}

void Modeller::saveFile(const std::string& path, const std::string& filename, bool selected) 
{
	std::string err; 
	Pi3CfileOBJ::save(path, filename, &scene, selected, nullptr, err);
}

void Modeller::saveScene(const std::string& path, const std::string& filename, bool selected, Pi3Cscene& scene)
{
	//Pi3CfileScene fs(path, filename, selected, scene);
}

bool Modeller::loadModelAt(const std::string& modelfile, const vec3f pos, const std::function<void(float)> showProgressCB)
{
	std::string ext = Pi3Cutils::getExt(modelfile);
	if (ext == "obj") {
		int32_t modelRef = scene.loadModelOBJ("", modelfile, pos, true, modelGroupId, false, showProgressCB);
		return true;
	}
	else if (ext == "fbx") {
		Pi3CopenFBX fbx(modelfile);
		fbx.importToScene(resource, &scene);
		return true;
	}
	else if (ext == "gltf" || ext == "glb") {
		//Pi3Cgltf gltf(resource, &scene, modelfile);
		Pi3Cfxgltf gltf(resource, &scene, modelfile, showProgressCB);
		return true;
	}
	return false;
}

void Modeller::moveSelections(const vec3f& vec)
{
	editUndo.moveSelections(scene.models, vec);
	sceneAction = SA_ZOOMING;
}

void Modeller::setCurrentSelView(int32_t selview)
{
	if (currentSelView != selview) currentSelView = selview;
}

void Modeller::dropMan()
{
	viewInfo& view = views[currentView];
	view.pos = vec3f(0, 0, 0);
	view.zoom = 0.f;
	view.pan = currentPos + vec3f(0,-2.f,0);
}

void Modeller::setFullScreen()
{
	if (fullscreen) {
		SDL_SetWindowFullscreen(window->handle(), 0);
		views[viewInfo::BOTTOMRIGHT].pan = views[viewInfo::FULLSCREEN].pan;
		views[viewInfo::BOTTOMRIGHT].rot = views[viewInfo::FULLSCREEN].rot;
		views[viewInfo::BOTTOMRIGHT].pos = views[viewInfo::FULLSCREEN].pos;
		views[viewInfo::BOTTOMRIGHT].zoom = views[viewInfo::FULLSCREEN].zoom;
		currentView = viewInfo::FULL;
		views[currentView] = views[viewInfo::BOTTOMRIGHT];
		setCurrentSelView(currentView);
	}
	else {

		int r = SDL_SetWindowFullscreen(window->handle(), SDL_WINDOW_FULLSCREEN);
		if (r < 0) {
			r = SDL_SetWindowFullscreen(window->handle(), SDL_WINDOW_FULLSCREEN_DESKTOP);
			if (r < 0) {
				SDL_SetWindowFullscreen(window->handle(), 0);
			}
			else {
				SDL_DisplayMode dm;
				SDL_GetCurrentDisplayMode(0, &dm);
				window->resizeWindow(dm.w, dm.h);
			}
		}
		else
		{
			window->resizeWindow(1440, 900);  //RPi can't seem to go fullscreen with a specified res - just drop it
		}

		currentView = viewInfo::FULLSCREEN;
		views[viewInfo::FULLSCREEN].pan = views[viewInfo::BOTTOMRIGHT].pan;
		views[viewInfo::FULLSCREEN].rot = views[viewInfo::BOTTOMRIGHT].rot;
		views[viewInfo::FULLSCREEN].pos = views[viewInfo::BOTTOMRIGHT].pos;
		views[viewInfo::FULLSCREEN].zoom = views[viewInfo::BOTTOMRIGHT].zoom;
		editMode = ED_ROTATESCENE;
		views[currentView].viewport = window->getRect();
		setCurrentSelView(currentView);
	}
	fullscreen = !fullscreen;

	//window->waitForMouseUp();
}

void Modeller::setFullScene()
{
	if (window->mouse.up) {
		if (fullview == viewInfo::INACTIVE) {
			fullview = currentSelView;
			views[viewInfo::FULL] = views[currentSelView];
			setCurrentSelView(viewInfo::FULL);
		}
		else {
			setCurrentSelView(fullview);
			fullview = viewInfo::INACTIVE;
		}
	}
	window->waitForMouseUp();
}

void Modeller::setCursor(SDL_Cursor *newCursor)
{
	if (newCursor != currentCursor) {
		currentCursor = newCursor;
		SDL_SetCursor(currentCursor);
	}
}

void Modeller::setDragBar(bool on, SDL_Cursor *newCursor)
{
	if (!selectRect) {
		if (on && sceneAction != SA_DRAGBAR) {
			sceneAction = SA_DRAGBAR;
			SDL_SetCursor(newCursor);
		}
		else if (!on && sceneAction == SA_DRAGBAR) {
			sceneAction = SA_NONE;
			SDL_SetCursor(currentCursor);
		}
	}
}

void Modeller::touchView(viewInfo& vi)
{
	switch (vi.projection) {
	case viewInfo::PERSPECTIVE: touchPerspectiveView(vi); break;
	case viewInfo::ORTHOGRAPHIC: touchOrthoView(vi); break;
	}
}

void Modeller::touchPerspectiveView(viewInfo &vi)
{
	scene.setMatrix(vi.pan, vec3f(0, 0, -vi.zoom), vi.rot);
	scene.setPerspective3D(vi.viewport.width, vi.viewport.height, vi.pspvalue, vi.psp_nearz, vi.psp_farz);
	float iy = window->getHeight() - window->mouse.y;
	vec3f mousexyz(vi.viewport.width*0.5f - (window->mouse.x - vi.viewport.x), vi.viewport.height*0.5f - (iy - vi.viewport.y), 0);
	touch = scene.touch(mousexyz, true);
}

void Modeller::touchOrthoView(viewInfo &vi)
{
	scene.setMatrix(vi.pos, vec3f(0, 0, 0), vi.rot);
	scene.setOrthographic3D(vi.viewport, vi.zoom, vi.ortho_nearz, vi.ortho_farz);
	vec3f mousexyz = -currentPos;
	touch = scene.touch(mousexyz, false);
}

void Modeller::setSelectionBox()
{
	Pi3Cbbox3d bbox = scene.getSelectedBounds();
	Pi3Cmodel& selBox = scene.models[selboxRef];
	vertsPtr vp = selBox.getMeshVerts(resource);
	Pi3Cgizmos::select_box_verts(*vp.verts, vp.offset, bbox.min, bbox.size(), 0xffffff);
	resource->updateMesh(selBox.meshRef);
}

void Modeller::setSelectionRect()
{
	Pi3Cmodel& srm = scene.models2D[selRectRef];
	vertsPtr vp = srm.getMeshVerts(resource);
	Pi3Cshapes::rectLine_verts(*vp.verts, vp.offset, vec2f((float)selRect.x, (float)selRect.y), vec2f((float)selRect.width, (float)selRect.height), vec2f(), vec2f(), 0xffffff);
	resource->updateMesh(srm.meshRef);
}

void Modeller::touchObject(Pi3Cmodel& selmodel)
{
	setSelectionBox();
	selectedName = selmodel.name;
	scene.models[moveGizmoRef].matrix.move(selmodel.bbox.center());
}

void Modeller::setTouchFlags(bool val)
{
	//scene.models[brushref].visible = val;
	scene.models[selboxRef].visible = val;
	scene.models[moveGizmoRef].visible = val;
}



void Modeller::clearSelections()
{
	editUndo.clearSelections(scene.models);
}

void Modeller::selectAll()
{
	editUndo.selectAll(scene.models);
}

void Modeller::animate()
{

}

void Modeller::setCreateTool(const CreateTool tool)
{
	createTool = tool;
	editMode = ED_CREATE;
	//mgui.startSnapshot();
	setCursor(crossCursor);
}

void Modeller::setEditMode(const EditMode mode)
{
	createTool = CT_NONE;
	editMode = mode;
	//mgui.startSnapshot();
}

void Modeller::setMousePosition(int x, int y)
{
	currentPos = views[currentView].calcMouseXYZ(x, y);
}

void Modeller::handleIMGUI()
{
	if (mgui.doIMGUI(this)) {
		prevCursor = handCursor;
		SDL_SetCursor(handCursor);
	}
	else if (prevCursor != currentCursor) {
		SDL_SetCursor(currentCursor);
		prevCursor = currentCursor;
	}
	//	SDL_SetCursor(currentCursor);
	//};

}

void Modeller::renderScene(viewInfo &view)
{
	view.ticks = window->getTicks();
	scene.renderView(view, &outlines);
}

Pi3Crecti Modeller::viewRect(const viewInfo::SceneLayout projection)
{
	switch (projection) {
		case viewInfo::FULL: return mgui.getRectFull();
		case viewInfo::BOTTOMRIGHT: return mgui.getRectBottomRight();
		case viewInfo::BOTTOMLEFT: return mgui.getRectBottomLeft();
		case viewInfo::TOPLEFT: return mgui.getRectTopLeft();
		case viewInfo::TOPRIGHT: return mgui.getRectTopRight();
	}
}

void Modeller::renderView(const viewInfo::SceneLayout projection, const Pi3Crecti& rect, int32_t mx, int32_t my)
{
	//if (refreshed() && (mx<rect.x || mx>(rect.x + rect.width) || my<rect.y || my>(rect.y + rect.height))) return;
	//window->clearRect(rect);
	views[projection].viewport = rect;
	if (views[projection].viewport.touch(mx, my)) currentView = projection;
	renderScene(views[projection]);
}

void Modeller::render()
{
	resource->calls = 0;

	if (fullscreen) {
		currentView = viewInfo::FULLSCREEN;
		renderScene(views[currentView]);
		return;
	}

	Pi3Crecti screenRect(0, 0, window->getWidth(), window->getHeight());
	int mx = window->mouse.x;
	int my = window->getHeight() - window->mouse.y;

	//Render 2D
	scene.setViewport(screenRect);
	scene.setFixedLight(0xffffff, vec3f(0, 1000.f, 1000.f));
	scene.setViewport2D(screenRect, 0.1f, 2000.f);
	//scene.render2D(window->getTicks());
	scene.setup2D();
	handleIMGUI(); //must be in the rendering loop with 2D setup

	currentView = viewInfo::INACTIVE;

	if (fullview >= 0) {
		renderView(viewInfo::FULL, mgui.getRectFull(), mx, my);
		//views[viewInfo::FULL].viewport = mgui.getRectFull();
		//currentView = viewInfo::FULL;
		//renderScene(views[viewInfo::FULL]);
	}
	else {
		renderView(viewInfo::BOTTOMRIGHT, mgui.getRectBottomRight(), mx, my); //render perspective view
		renderView(viewInfo::BOTTOMLEFT, mgui.getRectBottomLeft(), mx, my);
		renderView(viewInfo::TOPLEFT, mgui.getRectTopLeft(), mx, my);
		renderView(viewInfo::TOPRIGHT, mgui.getRectTopRight(), mx, my);
	}

	scene.setViewport(screenRect);
	scene.setup2D();
	//scene.render2D(window->getTicks());
	mgui.dragBars(this);

	if (currentView != viewInfo::INACTIVE) {
		std::string text = "X:" + Pi3Cutils::ftostrdp(currentPos.x, 2) + ", Y:" + Pi3Cutils::ftostrdp(currentPos.y, 2) + ", Z:" + Pi3Cutils::ftostrdp(currentPos.z, 2)+"    "+info;
		Pi3Crecti prect = viewRect((viewInfo::SceneLayout)currentSelView);
		mgui.printText(text, prect.x + 8, window->getHeight() - prect.y - 30, 0xffffffff);
	}
}

void Modeller::snapshot() {
	scene.renderOffscreen(views[currentSelView], &outlines);
}
