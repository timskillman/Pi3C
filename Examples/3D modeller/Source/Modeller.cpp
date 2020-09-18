#include "Modeller.h"
#include "Pi3Cshapes.h"
#include "Pi3Cdirectory.h"
#include <fstream>
#include <sstream>

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

	std::vector<std::string> dir = Pi3Cdirectory::readDirectory("../");

	// Setup scene ...
	scene.init(resource);
	scene.selectShader(basicShaderRef);
	scene.setViewport2D(Pi3Crecti(0, 0, window->getWidth(), window->getHeight()),-200.f, 2000.f);
	window->setClearColour(0xff804040);
	
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

	mgui.init(opts, resource, window);

	views[viewInfo::TOPLEFT] = setupView(viewInfo::VIEW_FRONT);
	views[viewInfo::TOPRIGHT] = setupView(viewInfo::VIEW_LEFT);
	views[viewInfo::BOTTOMLEFT] = setupView(viewInfo::VIEW_TOP);
	views[viewInfo::BOTTOMRIGHT] = setupView(viewInfo::VIEW_PERSPECTIVE);
	views[viewInfo::FULL] = setupView(viewInfo::VIEW_PERSPECTIVE);
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
	Pi3Cavatar::avatarParams avparams;
	avparams.movement = Pi3Cavatar::move_walk;
	avparams.position = opts.asVec3f("startpos");
	avparams.rotation = opts.asVec3f("rotation");
	avparams.size = { 1.f, opts.asFloat("avatarHeight"), 1.f };
	avparams.walkSpeed = opts.asFloat("avatarWalkSpeed");
	avparams.runSpeed = opts.asFloat("avatarRunSpeed");
	avparams.fallSpeed = opts.asFloat("avatarFallSpeed");
	player.init(avparams);

	// Load Skybox ...
	skybox = scene.loadModelOBJ(opts.asString("skyboxPath"), opts.asString("skybox"), vec3f(0,0,0), true, nullptr); // loadbarCallback);
	scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
	scene.models[skybox].touchable = false;

	// Create a brush for touching objects ...
	Pi3Cmodel brush = Pi3Cmodel(resource, Pi3Cshapes::sphere(vec3f(0, 0, 0.f), 0.5f, 0.f, 10, 10), 0xff00ffff);
	brush.touchable = false;
	brush.visible = false;
	brushref = scene.append3D(brush);

	// Create grid
	Pi3Cmodel gridModel = Pi3Cmodel(resource, Pi3Cshapes::grid(vec2f(100.f, 100.f), 10.f, 1.f, 0x606060, 0x303030));
	gridModel.touchable = false;
	gridModel.material.rendermode = GL_LINE_STRIP;
	gridRef = scene.append3D(gridModel);

	// Create selection box
	Pi3Cmodel selboxModel = Pi3Cmodel(resource, Pi3Cgizmos::selectBoxGizmo(vec3f(0, 0, 0), vec3f(0, 0, 0), 0xffffff));
	selboxModel.touchable = false;
	selboxModel.material.rendermode = GL_LINE_STRIP;
	selboxRef = scene.append3D(selboxModel);

	// Create move gizmo
	Pi3Cmodel moveGizmo = Pi3Cmodel(resource, Pi3Cgizmos::moveGizmo(vec3f(0, 0, 0), vec3f(0, 0, 0), 0xffffffff));
	moveGizmo.touchable = false;
	moveGizmo.visible = false;
	moveGizmo.material.rendermode = GL_LINES;
	moveGizmo.material.illum = 0;
	moveGizmoRef = scene.append3D(moveGizmo);

	outlines = *resource->defaultMaterial();
	outlines.rendermode = GL_LINES | GL_LINE_STRIP;
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

void Modeller::handleKeyPresses()
{
	switch (window->getKeyPress()) {
	case SDL_SCANCODE_DELETE:
		editUndo.deleteSelection(scene.models);
		clearGizmos();
		break;
	case SDL_SCANCODE_X:
		if (window->ctrlKey) editUndo.deleteSelection(scene.models);
		break;
	case SDL_SCANCODE_Z:
		if (window->ctrlKey) {
			if (window->shiftKey) 
				editUndo.redo(scene.models); 
			else 
				editUndo.undo(scene.models);
		}
		break;
	case SDL_SCANCODE_Y:
		if (window->ctrlKey) editUndo.redo(scene.models);
		break;
	case SDL_SCANCODE_A:
		if (window->ctrlKey) selectAll();
		break;
	case SDL_SCANCODE_G:
		scene.models[gridRef].visible = !scene.models[gridRef].visible;
		window->mouse.up = false;
		break;
	case SDL_SCANCODE_P:
		scene.renderOffscreen(views[currentSelView], &outlines);
		window->mouse.up = false;
		break;
	case SDL_SCANCODE_T:
		scene.renderOffscreen(views[currentSelView], &outlines, 160, 128);
		window->mouse.up = false;
		break;
	}
	//keyPress = ev.key.keysym.scancode;
}

void Modeller::createLandscape(const vec3f pos, const uint32_t colour)
{
	Pi3Ctexture maptex = Pi3Ctexture("assets/maps/mountainsHgt2.png", false);
	createShape(Pi3Cshapes::elevationMap(maptex, vec3f(0, -200.f, 0), vec3f(3000.f, 400.f, 3000.f), 128, 128, 0), pos, colour);
}

void Modeller::createShape(const Pi3Cmesh& mesh, const vec3f& pos, const uint32_t colour)
{
	Pi3Cmodel shape = Pi3Cmodel(resource, mesh, colour);
	if (shape.meshRef >= 0) {
		clearSelections();
		//Give unique name for each shape ...
		shapeCounts[createTool]++;
		shape.name = resource->meshes[shape.meshRef].name + std::to_string(shapeCounts[createTool]);
		shape.move(pos);
		shape.selected = true;
		int32_t modelRef = scene.append3D(shape);
		if (modelRef >= 0) editUndo.create(scene.models);
	}
}

void Modeller::createShapes()
{
	if (createTool != CT_NONE) {
		currentColour = rand() % 0xffffff;
		vec3f pos = -currentPos;
		switch (createTool) {
		case CT_CUBOID: createShape(Pi3Cshapes::cuboid(vec3f(0, 0, 0.f), vec3f(2.f, 2.f, 2.f)), pos, currentColour); break;
		case CT_CYLINDER: createShape(Pi3Cshapes::cylinder(vec3f(0, 0, 0.f), 1.f, 2.f), pos, currentColour); break;
		case CT_TUBE: createShape(Pi3Cshapes::tube(vec3f(0, 0, 0.f), 0.5f, 1.f, 2.f), pos, currentColour); break;
		case CT_CONE: createShape(Pi3Cshapes::cone(vec3f(0, 0, 0.f), 1.f, 2.f), pos, currentColour); break;
		case CT_TCONE: createShape(Pi3Cshapes::tcone(vec3f(0, 0, 0.f), 1.f, 0.7f, 2.f), pos, currentColour); break;
		case CT_SPHERE: createShape(Pi3Cshapes::sphere(vec3f(0, 0, 0.f), 1.f), pos, currentColour); break;
		case CT_TORUS: createShape(Pi3Cshapes::torus(vec3f(0, 0, 0.f), 2.f, 1.f), pos, currentColour); break;
		case CT_WEDGE: break;
		case CT_EXTRUDE: break;
		case CT_LATHE: break;
		case CT_LANDSCAPE: createLandscape(pos, currentColour); break;
		}
	}
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
		view.rot += vec3f(mouseXYZ.y, mouseXYZ.x, 0) * -0.01f;
		sceneAction = SA_ROTATING;
		//rotating = true;
		break;
	}
}

void Modeller::ClickLeftMouseButton(viewInfo& view)
{
	if (currentViewIsActive() && window->mouse.LeftButton && sceneAction != SA_DRAGBAR) {

		setCurrentSelView(currentView);

		switch (editMode) {
		case ED_CREATE:
			createShapes();
			window->mouse.up = false;
			break;
		case ED_SELECT:
			touchScene();
			if (touch.selmodel) {
				scene.models[brushref].matrix.move(touch.intersection);
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

	if (currentViewIsActive() && !mgui.somethingSelected()) {

		touchView(views[currentView]);

		if (editMode == ED_CREATE) setCursor(crossCursor);

		//Pi3Cmodel& brush = scene.models[brushref];
		//Pi3Cmodel& selmodel = scene.models[touch.parent()];

		if (touch.selmodel) {

			//move 3D pointer (sphere) to touch intersetion point ...
			currentPos = -touch.intersection;

			//scene.models[brushref].matrix.move(touch.intersection);
			setTouchFlags(true);
			touch.selmodel->selected = true;

			switch (editMode) {
			case ED_SELECT: touchObject(*touch.selmodel); break;
			case ED_MOVE: setCursor(moveCursor); break;
			case ED_ROTATE: setCursor(handCursor); break;
			case ED_SCALE: setCursor(handCursor); break;
			case ED_DROPMAN: dropMan(); break;
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

void Modeller::handleEvents(std::vector<uint32_t>& eventList)
{
	if (eventList.size() == 0) return;

	// process window events ...
	viewInfo& view = views[currentView];

	for (auto& ev : eventList) {
		switch (ev)
		{
		case SDL_MOUSEBUTTONDOWN:
			ClickLeftMouseButton(view);
			break;
		case SDL_MOUSEBUTTONUP:
			MouseButtonUp();
			break;
		case SDL_MOUSEMOTION:
			if (window->mouse.anyButton() && currentViewIsActive() && sceneAction != SA_DRAGBAR) {
				vec3f mouseXYZ = vec3f(window->mouse.deltaXY.x, -window->mouse.deltaXY.y, 0);
				//float sc = -window->mouse.deltaXY.y * 0.1f;
				if (window->mouse.MiddleButton) DragMiddleMouseButton(view, mouseXYZ);
				if (window->mouse.LeftButton) DragLeftMouseButton(view, mouseXYZ);
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
			}
			break;
		case SDL_KEYDOWN:
			handleKeyPresses();
			break;
		case SDL_DROPFILE: 
			std::string file = window->dropfile;
			if (file.substr(file.size() - 4, 4) == ".obj") {
				int32_t modelRef = scene.loadModelOBJ("", file, touch.touching ? touch.intersection : vec3f(0,0,0), true, nullptr);  // loadbarCallback);
			}
			break;
		}
	}

}

void Modeller::moveSelections(const vec3f& vec)
{
	editUndo.moveSelections(scene.models, vec);
	sceneAction = SA_ZOOMING;
}

void Modeller::setCurrentSelView(int32_t selview)
{
	if (currentSelView != selview) {
		//if (selview != viewInfo::TOPLEFT) scene.snapShot(viewInfo::TOPLEFT);
		currentSelView = selview;
	}
}

void Modeller::dropMan()
{
	viewInfo& view = views[currentView];
	view.pos = vec3f(0, 0, 0);
	view.zoom = 2.f;
	view.pan = currentPos;
}

void Modeller::setFullScene()
{
	if (fullview == viewInfo::INACTIVE) {
		fullview = currentSelView;
		views[viewInfo::FULL] = views[currentSelView];
		setCurrentSelView(viewInfo::FULL);
	}
	else {
		setCurrentSelView(fullview);
		fullview = viewInfo::INACTIVE;
	}
	window->mouse.up = false;
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
	if (on && sceneAction != SA_DRAGBAR) {
		sceneAction = SA_DRAGBAR;
		SDL_SetCursor(newCursor);
	}
	else if (!on && sceneAction == SA_DRAGBAR) {
		sceneAction = SA_NONE;
		SDL_SetCursor(currentCursor);
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
	vertsPtr vp = resource->getMeshVerts(scene.models[selboxRef].meshRef);
	Pi3Cgizmos::select_box_verts(*vp.verts, vp.ptr, bbox.min, bbox.size(), 0xffffff);
	resource->updateMesh(scene.models[selboxRef].meshRef);
}

void Modeller::touchObject(Pi3Cmodel& selmodel)
{
	//setCursor(handCursor);
	setSelectionBox();
	selectedName = selmodel.name;
	scene.models[moveGizmoRef].matrix.move(selmodel.bbox.center());
}

void Modeller::touchView(viewInfo &vi)
{
	switch (vi.projection) {
	case viewInfo::PERSPECTIVE: touchPerspectiveView(vi); break;
	case viewInfo::ORTHOGRAPHIC: touchOrthoView(vi); break;
	}
}

void Modeller::setTouchFlags(bool val)
{
	scene.models[brushref].visible = val;
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
}

void Modeller::setEditMode(const EditMode mode)
{
	createTool = CT_NONE;
	editMode = mode;
}

void Modeller::handleIMGUI()
{
	mgui.doIMGUI(this);
}

void Modeller::renderScene(viewInfo &view)
{
	view.ticks = window->getTicks();
	scene.renderView(view, &outlines);
}

void Modeller::render()
{
	resource->calls = 0;

	Pi3Crecti screenRect(0, 0, window->getWidth(), window->getHeight());
	int mx = window->mouse.x;
	int my = window->getHeight() - window->mouse.y;

	currentView = viewInfo::INACTIVE;

	if (fullview >= 0) {
		views[viewInfo::FULL].viewport = mgui.getRectFull();
		currentView = viewInfo::FULL;
		renderScene(views[viewInfo::FULL]);
	}
	else {
		//render perspective view (right lower)...
		
		views[viewInfo::BOTTOMRIGHT].viewport = mgui.getRectBottomRight();
		if (views[viewInfo::BOTTOMRIGHT].viewport.touch(mx, my)) currentView = viewInfo::BOTTOMRIGHT; 
		renderScene(views[viewInfo::BOTTOMRIGHT]);

		//render left lower ...
		views[viewInfo::BOTTOMLEFT].viewport = mgui.getRectBottomLeft();
		if (views[viewInfo::BOTTOMLEFT].viewport.touch(mx, my)) currentView = viewInfo::BOTTOMLEFT;
		renderScene(views[viewInfo::BOTTOMLEFT]);

		//render left top ...
		views[viewInfo::TOPLEFT].viewport = mgui.getRectTopLeft();
		if (views[viewInfo::TOPLEFT].viewport.touch(mx, my)) currentView = viewInfo::TOPLEFT;
		renderScene(views[viewInfo::TOPLEFT]);

		//render right top ...
		views[viewInfo::TOPRIGHT].viewport = mgui.getRectTopRight();
		if (views[viewInfo::TOPRIGHT].viewport.touch(mx, my)) currentView = viewInfo::TOPRIGHT;
		renderScene(views[viewInfo::TOPRIGHT]);
	}

	//Render 2D
	scene.setViewport(screenRect);
	scene.setFixedLight(0xffffff, vec3f(0, 1000.f, 1000.f));
	scene.setViewport2D(screenRect, 0.1f, 2000.f);
	scene.render2D(window->getTicks());

	handleIMGUI(); //must be in the rendering loop with 2D setup

}
