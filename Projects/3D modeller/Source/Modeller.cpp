#include "Modeller.h"
#include "Pi3Cshapes.h"
#include "Pi3Cdirectory.h"
#include <fstream>
#include <sstream>

Modeller::Modeller(Pi3Cresource *resource, Pi3Cwindow *window)
{
	// Setup shader ..
	int32_t basicShaderRef = resource->addShader("Shaders/vs1.glsl", "Shaders/fs1.glsl");
	if (basicShaderRef < 0) {
		window->showErrorMessage("Shader Error!", resource->error());
		return;
	}

	this->resource = resource;
	this->window = window;

	gui.init(resource, window);

	std::vector<std::string> dir = Pi3Cdirectory::readDirectory("../");

	// Setup scene ...
	scene.resource = resource;
	scene.selectShader(basicShaderRef);
	scene.setViewport2D(Pi3Crecti(0, 0, window->getWidth(), window->getHeight()), 0.1f, 2000.f);
	window->setClearColour(0xff804040);

}

void Modeller::newScene(const uint32_t ref)
{
	Pi3Cmodel sceneModel;
	if (scene.models.size() > 0)
		scene.models[0] = sceneModel;
	else
		scene.models.push_back(sceneModel);
}

void Modeller::setupGUI(loadOptions &opts)
{
	// Get GUI fonts
	//resource->addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 80);
	guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 28));
	guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 16));

	gui.setImagePath("icons");
	//gui.loadImages("../../Resources/models/maps", { "larrow.png","rarrow.png","darrow.png","uarrow.png","handle.png","radioon.png","radiooff.png" });
	//gui.loadImages("icons", { "cuboid.png","cylinder.png","tube.png","cone.png","t-cone.png","extrude.png","lathe.png","sphere.png","wedge.png" });
	//gui.loadImages("icons", { "rendl.png","rendr.png", "butDiv.png", "butNew.png","butOpen.png","butSave.png","butArrow.png","butCopy.png","butCut.png","butPaste.png","butFlip.png","butMirror.png","butUndo.png","butRedo.png","butSelectAll.png" });

	uint32_t backColour = 0xcccccc;
	uint32_t white = 0xffffff;
	uint32_t highlight = 0xeeeeee;
	uint32_t selectColour = 0x00ffff;

	// Setup GUI button styles ...
	bsMenu.font = guifonts[1];
	bsMenu.textColour = 0x0;
	bsMenu.buttonAlpha = 1.f;
	bsMenu.buttonColour = white;
	bsMenu.minHeight = 32;
	bsMenu.vertGap = 0;
	bsMenu.left = 5;
	bsMenu.highlightColour = 0xc0c0c0;

	bsHeading.font = guifonts[0];
	bsHeading.textColour = white;
	bsHeading.highlightColour = 0x00ffff;
	bsHeading.buttonColour = backColour;
	bsHeading.top = 0;
	bsHeading.bottom = 0;
	bsHeading.left = 0;
	bsHeading.right = 0;

	bsIcons.font = guifonts[0];
	bsIcons.textColour = white;
	bsIcons.buttonColour = backColour;
	bsIcons.highlightColour = highlight;
	bsIcons.minWidth = 32;
	bsIcons.minHeight = 32;
	bsIcons.top = 0;
	bsIcons.bottom = 0;
	bsIcons.left = 0;
	bsIcons.right = 0;
	bsIcons.horizGap = 0;
	bsIcons.sameLine = true;
	bsIcons.selectColour = selectColour;

	bsButtons.font = guifonts[0];
	bsButtons.textColour = white;
	bsButtons.buttonColour = backColour;
	bsButtons.highlightColour = highlight;
	bsButtons.justify = Pi3Cimgui::CENTRE;
	bsButtons.align = Pi3Cimgui::CENTRE;
	bsButtons.minWidth = 40;
	bsButtons.minHeight = 40;
	bsButtons.top = 1;
	bsButtons.bottom = 1;
	bsButtons.left = 0;
	bsButtons.right = 0;
	bsButtons.vertGap = 0;
	bsButtons.selectColour = selectColour;
	//bsButtons.sameLine = true;

	bsItems.font = guifonts[1];
	bsItems.buttonAlpha = 0.8f;
	bsItems.buttonColour = white;
	bsItems.textColour = 0x404040;
	bsItems.minWidth = 140;
	bsItems.minHeight= 40;
	bsItems.justify = Pi3Cimgui::CENTRE;
	bsItems.align = Pi3Cimgui::BOTTOM;
	bsItems.selectColour = selectColour;

	views[TOPLEFT] = setupView(VIEW_FRONT);
	views[TOPRIGHT] = setupView(VIEW_LEFT);
	views[BOTTOMLEFT] = setupView(VIEW_TOP);
	views[BOTTOMRIGHT] = setupView(VIEW_PERSPECTIVE);
}

void Modeller::init()
{
	// Load 3D models and scene ...
	loadOptions opts("options.txt");

	setupGUI(opts);

	nearzfarz = opts.asVec2f("nearzfarz");
	scene.setFog(0xffffff, 10000.f, 25000.f);
	scene.setPerspective3D(window->getWidth(), window->getHeight(), 800.f, nearzfarz.x, nearzfarz.y);

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
	skybox = scene.loadModelOBJ(opts.asString("skyboxPath"), opts.asString("skybox"), 0); // loadbarCallback);
	scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
	scene.models[skybox].touchable = false;

	// Create a brush for touching objects ...
	Pi3Cmodel torus = Pi3Cmodel(resource, Pi3Cshapes::torus(vec3f(0, 0, 0.f), 7.f, 2.f,24,24), 0xff8080);
	scene.append3D(torus);

	Pi3Cmodel brush = Pi3Cmodel(resource, Pi3Cshapes::sphere(vec3f(0, 0, 0.f), 2.0f, 0.f, 10, 20), 0xff00ffff);
	brush.touchable = false;
	brushref = scene.append3D(brush);

	// Upload meshes to GPU after all geometries have been created ...
	resource->uploadMeshesToGPU();

}

Modeller::viewInfo Modeller::setupView(const ViewProject proj)
{
	const static float PIDIV2 = PI / 2.f;
	const static vec3f viewDir[8] = { { 0, PIDIV2, 0 }, { 0, -PIDIV2, 0 }, { PIDIV2, 0, 0 },{ -PIDIV2, 0, 0 },{}, { 0, PI, 0 },{ .7f, .7f, 0 },{ .7f, .7f, 0 } };

	viewInfo view;
	view.viewProject = proj;
	view.projection = (proj==VIEW_PERSPECTIVE) ? PERSPECTIVE : ORTHOGRAPHIC;
	view.setRot(viewDir[proj]);
	return view;
}

void Modeller::handleKeys()
{
	//player.moved = false;
	//const uint8_t *keystate = window->getKeys();
	//player.run(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]);
	//if (keystate[SDL_SCANCODE_W]) if (editMode) editpos.y += 5.f; else player.forward();
	//if (keystate[SDL_SCANCODE_S]) if (editMode) editpos.y -= 5.f; else player.back();
	//if (keystate[SDL_SCANCODE_A]) if (editMode) editpos.x += 5.f; else player.left();
	//if (keystate[SDL_SCANCODE_D]) if (editMode) editpos.x -= 5.f; else player.right();
	//if (keystate[SDL_SCANCODE_R]) if (editMode) editpos.z += 5.f; else player.up();			//only when flying
	//if (keystate[SDL_SCANCODE_F]) if (editMode) editpos.z -= 5.f; else player.down();		//only when flying
	//if (keystate[SDL_SCANCODE_SPACE]) player.jump();	//only when walking
	//if (keystate[SDL_SCANCODE_ESCAPE]) window->setquit(true);

	//if (!keypress) {
	//	if (!modeChange && keystate[SDL_SCANCODE_V]) {
	//		editMode = !editMode;
	//		if (editMode) edTransition = 0.99f; else edTransition = 0.01f;
	//		SDL_Log("edTransition = %f", edTransition);
	//		modeChange = true;
	//		keypress = true;
	//	}
	//	if (keystate[SDL_SCANCODE_G]) { 
	//		gridlock = !gridlock; 
	//		uint32_t col = (gridlock) ? 0xff00ffff : 0xffff000;
	//		scene.models[brushref].material.SetColDiffuse(col);
	//		keypress = true; 
	//	}
	//	if (keystate[SDL_SCANCODE_DELETE]) {
	//		if (selGroup) {
	//			Pi3Cmodel &touchMod = scene.models[touch.groupRefs[0]].group[touch.groupRefs[1]];
	//			touchMod.deleted = true;
	//		}
	//		keypress = true;
	//	}
	//}
	//else if (!window->keydown()) keypress = false;

	//player.updateAndCollide(&scene, window->getTicks());
}

void Modeller::handleEvents()
{
	// process window events ...
	while (window->event()) {
		switch (window->ev.type)
		{
		case SDL_MOUSEMOTION:
			if (window->mouse.anyButton()) {
				if (window->mouse.MiddleButton && currentView >= 0) {
					float iz = (views[currentView].projection == ORTHOGRAPHIC) ? 2.f / views[currentView].zoom : -.1f;
					//if (views[currentView].projection == ORTHOGRAPHIC) {
					//	float iz = 1 / views[currentView].zoom * 2.f;
						views[currentView].pan -= views[currentView].rotInvertMatrix.transformRotateVec(vec3f(window->mouse.deltaXY.x*iz, -window->mouse.deltaXY.y*iz, 0));
					//}
					//else {
					//	views[currentView].pan += views[currentView].rotInvertMatrix.transformRotateVec(vec3f(window->mouse.deltaXY.x*.1f, -window->mouse.deltaXY.y*.1f, 0));
					//}
					//vec3f rotateVec(window->mouse.deltaXY.y / 300.f, -window->mouse.deltaXY.x / 300.f, 0);
					//views[currentView].rot += rotateVec;
				}
			}
			break;
		case SDL_MOUSEWHEEL:
			if (currentView >= 0) {
				//if ((views[currentView].zoom + window->mouse.wheel * 20.f) > 1.f) 
					views[currentView].zoom += window->mouse.wheel * 10.f;
			}
			break;
		case SDL_WINDOWEVENT:
			switch (window->ev.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				//scene.setPerspective3D(800.f, nearzfarz.x, nearzfarz.y);
				//scene.setViewport2D(Pi3Crect(0, 0, (float)window->getWidth(), (float)window->getHeight()), 0.1f, 2000.f);
				break;
			}
			break;
		}
	}
}

void Modeller::touchScene()
{
	selGroup = nullptr;
	//scene.models[brushref].visible = false;

	if (!player.moved && !gui.somethingSelected) {

		vec3f mousexyz(window->getWidth()*0.5f - window->mouse.x, window->mouse.y - window->getHeight()*0.5f, 0);
		touch = scene.touch3D(mousexyz);

		if (touch.touched()) {

			////move 3D pointer (sphere) to touch intersetion point ...
			//scene.models[brushref].matrix.move(touch.intersection);
			//scene.models[brushref].visible = true;

			//if (touch.maxlevel > 1) {

			//	if (window->mouse.LeftButton && window->mouse.up && currentSel<modelsLib->group.size()) {
			//		Pi3Cmodel newmod = modelsLib->group[currentSel];
			//		if (gridlock) {
			//			//Replace selected object with library object
			//			Pi3Cmodel &touchMod = scene.models[touch.groupRefs[0]].group[touch.groupRefs[1]];
			//			newmod.matrix = touchMod.matrix;		//use old matrix of object to replace
			//			touchMod = newmod;
			//		}
			//		else {
			//			//Insert library object as touch position
			//			newmod.matrix.move(touch.intersection);
			//			scene.models[0].append(newmod);
			//		}
			//		window->mouse.up = false;
			//	}
			//	else if (window->mouse.wheel) {
			//		//Rotate selected object ...
			//		Pi3Cmodel *rotGroup = scene.getSubModel(&touch.groupRefs[0], 1);
			//		Pi3Cmatrix rotmtx;
			//		float sgn = (float)window->mouse.wheel;
			//		float rotateBy = (!gridlock) ? (PI / 8.f)*sgn : (PI / 2.f)*sgn;
			//		rotmtx.SetRotateYbit(rotateBy);
			//		rotGroup->matrix = rotmtx * rotGroup->matrix;
			//		window->mouse.wheel = 0;
			//	}
			//	else if (!window->mouse.anyButton()) {
			//		selGroup = scene.getSubModel(&touch.groupRefs[0], 2);
			//		//SDL_Log("Intersect = %f,%f,%f, %s", touch.intersection.x, touch.intersection.y, touch.intersection.z, selGroup->name.c_str());

			//		lastCol = touch.selmodel->material.colAmbient;
			//		for (auto &m : selGroup->group) m.material.SetColAmbient(0xffffffff);
			//	}
			//}
		}
	}
}

void Modeller::animate()
{
	//body.walk(vec3f(0, 0, 0), 0.05f * window->getTicks());
}

void Modeller::setCreateTool(CreateTool tool)
{
	createTool = tool;
}

void Modeller::handleIMGui()
{
	gui.Begin();

	//Menubar ...
	gui.setButtonStyle(bsMenu);
	if (gui.BeginMenuBar()) {
		if (gui.BeginMenu("File")) {
			if (gui.MenuItem("New", "Ctrl+N")) {} // newScene(sceneModelRef);
			if (gui.MenuItem("Open", "Ctrl+O")) {} // open();
			if (gui.MenuItem("Save", "Ctrl+S")) {}
			if (gui.MenuItem("Quit", "Esc")) window->setquit(true);
			gui.EndMenu();
		}
		if (gui.BeginMenu("Edit")) {
			if (gui.MenuItem("Copy", "Ctrl+C")) {}
			if (gui.MenuItem("Paste", "Ctrl+V")) {}
			if (gui.MenuItem("Delete", "Del")) {}
			if (gui.MenuItem("Undo", "Ctrl+X")) {}
			if (gui.MenuItem("Redo", "Ctrl+Y")) {}
			gui.EndMenu();
		}
		if (gui.BeginMenu("Object")) {
			if (gui.MenuItem("Create", "Ctrl+C")) {}
			if (gui.MenuItem("Array tool", "Ctrl+V")) {}
			if (gui.MenuItem("Delete", "Del")) {}
			if (gui.MenuItem("Undo", "Ctrl+X")) {}
			if (gui.MenuItem("Redo", "Ctrl+Y")) {}
			gui.EndMenu();
		}
		gui.EndMenuBar();
	}

	int hw = window->getWidth();
	int hh = window->getHeight();
	int workWidth = hw - leftbarWidth - rightbarWidth;
	int workHeight = hh - menuHeight - topbarHeight - botbarHeight;
	bool mb = window->mouse.LeftButton;

	//Draw borders ...
	gui.renderRect(hw, topbarHeight);
	gui.nextLine(0);
	Pi3Cpointi wpos = gui.getPosition();
	int midht = hh - wpos.y - botbarHeight;
	gui.renderRect(leftbarWidth, midht);
	gui.movePosition(hw - rightbarWidth, 0);
	gui.renderRect(rightbarWidth, midht);
	gui.setPosition(0, wpos.y + midht);
	gui.renderRect(hw, botbarHeight);

	//Drag bar horizontal ...
	gui.setPosition(leftbarWidth + (int)(dragBarX * (float)workWidth - dragBarThickness * 0.5f), wpos.y);
	if (gui.renderRect((int)dragBarThickness, midht) && mb && !draggingBarX && !draggingBarY) {
		draggingBarX = true;
	}
	else if (draggingBarX && mb) {
		dragBarX = (float)(window->mouse.x - leftbarWidth) / (float)workWidth;
		if (dragBarX < 0.02f) dragBarX = 0.02f;
		if (dragBarX > 0.98f) dragBarX = 0.98f;
	}
	else draggingBarX = false;

	//Drag bar vertical ...
	gui.setPosition(leftbarWidth, topbarHeight + menuHeight + (int)((float)workHeight*dragBarY - dragBarThickness*0.5f));
	if (gui.renderRect(workWidth, (int)dragBarThickness) && mb && !draggingBarX && !draggingBarY) {
		draggingBarY = true;
	}
	else if (draggingBarY && mb) {
		dragBarY = (float)(window->mouse.y - topbarHeight - menuHeight) / (float)workHeight;
		if (dragBarY < 0.02f) dragBarY = 0.02f;
		if (dragBarY > 0.98f) dragBarY = 0.98f;
	}
	else draggingBarY = false;

	//Top menu bar icons ...
	int icw = (int)((float)bsIcons.minWidth*0.5f);
	int ich = bsIcons.minHeight;
	int idw = (int)((float)bsIcons.minWidth*0.3f);
	gui.setButtonStyle(bsIcons);

	gui.setPosition(wpos.x + leftbarWidth, wpos.y - topbarHeight);
	if (gui.Container("EditButton", 200, 32)) {
		gui.renderBackIcon("rendl.png", icw, ich);
		if (gui.ButtonImage("butNew.png") && mb) {}
		if (gui.ButtonImage("butOpen.png") && mb) {}
		if (gui.ButtonImage("butSave.png") && mb) {}
		gui.renderBackIcon("butDiv.png", idw, ich);
		if (gui.ButtonImage("butArrow.png", editMode == EM_SELECT) && mb) editMode = EM_SELECT;
		if (gui.ButtonImage("butMove.png", editMode == EM_MOVE) && mb) editMode = EM_MOVE;
		if (gui.ButtonImage("butRotate.png", editMode == EM_ROTATE) && mb) editMode = EM_ROTATE;
		if (gui.ButtonImage("butScale.png", editMode == EM_SCALE) && mb) editMode = EM_SCALE;
		gui.renderBackIcon("butDiv.png", idw, ich);
		if (gui.ButtonImage("butSelectAll.png") && mb) {}
		if (gui.ButtonImage("butCut.png") && mb) {}
		if (gui.ButtonImage("butPaste.png") && mb) {}
		if (gui.ButtonImage("butCopy.png") && mb) {}
		gui.renderBackIcon("butDiv.png", idw, ich);
		if (gui.ButtonImage("butUndo.png") && mb) {}
		if (gui.ButtonImage("butRedo.png") && mb) {}
		gui.renderBackIcon("rendr.png", icw, ich);
		gui.sameLine(3);
		gui.renderBackIcon("rendl.png", icw, ich);
		if (gui.ButtonImage("butMirror.png") && mb) {}
		if (gui.ButtonImage("butFlip.png") && mb) {}
		gui.renderBackIcon("rendr.png", icw, ich);

		gui.ContainerEnd("EditButton");
	}

	//Left side create shape buttons ...
	gui.setPosition(wpos.x+(leftbarWidth-bsButtons.minWidth)/2, wpos.y);
	bsButtons.sameLine = false;
	gui.setButtonStyle(bsButtons);
	
	int btop = (int)((float)bsButtons.minWidth * .34f);
	gui.renderBackIcon("butTop2.png", bsButtons.minWidth, btop);
	if (gui.ButtonImage("cuboid.png", createTool == CT_CUBOID) && mb) setCreateTool(CT_CUBOID);
	if (gui.ButtonImage("sphere.png", createTool == CT_SPHERE) && mb) setCreateTool(CT_SPHERE);
	if (gui.ButtonImage("cone.png", createTool == CT_CONE) && mb) setCreateTool(CT_CONE);
	if (gui.ButtonImage("tube.png", createTool == CT_TUBE) && mb) setCreateTool(CT_TUBE);
	if (gui.ButtonImage("t-cone.png", createTool == CT_TCONE) && mb) setCreateTool(CT_TCONE);
	if (gui.ButtonImage("wedge.png", createTool == CT_WEDGE) && mb) setCreateTool(CT_WEDGE);
	gui.renderBackIcon("butBot2.png", bsButtons.minWidth, btop);
	gui.renderBackIcon("butTop2.png", bsButtons.minWidth, btop);
	if (gui.ButtonImage("extrude.png", createTool == CT_EXTRUDE) && mb) setCreateTool(CT_EXTRUDE);
	if (gui.ButtonImage("lathe.png", createTool == CT_LATHE) && mb) setCreateTool(CT_LATHE);
	gui.renderBackIcon("butBot2.png", bsButtons.minWidth, btop);
	

	gui.setPosition(leftbarWidth, workHeight+topbarHeight+menuHeight+3);
	gui.setButtonStyle(bsHeading);
	static double v = 5;
	gui.SliderH("Scroll", 0, 10, v, 300, 24);

	Pi3Cmodel textHello;
	std::string hello{ "Hello this is a long line of text!" };
	textHello.textModel(resource, gui.getFont(guifonts[0]).get(), hello, 400);
}

void Modeller::renderScene(const viewInfo &view)
{
	Pi3Cmaterial outlines;
	outlines = *resource->defaultMaterial();
	outlines.SetColDiffuse(0x00ffff);
	outlines.rendermode = GL_LINE_LOOP;
	outlines.illum = 1;

	scene.setViewport(view.viewport);
	switch (view.projection)
	{
	case PERSPECTIVE:
		scene.setMatrix(view.pan, vec3f(0,0,-view.zoom), view.rot);
		scene.setPerspective3D(view.viewport.width, view.viewport.height, 800.f, nearzfarz.x, nearzfarz.y);
		scene.setSun(0xffffff, vec3f(1000.f, 1000.f, -1000.f)); //transform sun position into scene
		scene.render3D(window->getTicks());
		break;
	case ORTHOGRAPHIC:
		scene.setMatrix(view.pos + view.pan, vec3f(0,0,0), view.rot);
		scene.setOrthographic3D(view.viewport, 1.f/ view.zoom, 0.1f, 2000.f);
		scene.setFixedLight(0xffffff, vec3f(1000.f, 1000.f, 1000.f));
		scene.render3D(window->getTicks(), &outlines);
		break;
	}
}

void Modeller::render()
{
	resource->calls = 0;

	//Render 3D
	float ibx = 1.f - dragBarX;
	float iby = 1.f - dragBarY;
	int hw = window->getWidth();
	int hh = window->getHeight();
	int workWidth = hw - leftbarWidth - rightbarWidth;
	int workHeight = hh - menuHeight - topbarHeight - botbarHeight;
	int mx = window->mouse.x;
	int my = hh - window->mouse.y;
	currentView = -1;

	//render perspective view (right lower)...
	views[BOTTOMRIGHT].viewport = Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX), botbarHeight, (int)((float)workWidth*ibx), (int)((float)workHeight*iby));
	if (views[BOTTOMRIGHT].viewport.touch(mx, my)) currentView = BOTTOMRIGHT;
	renderScene(views[BOTTOMRIGHT]);

	//render left lower ...
	views[BOTTOMLEFT].viewport = Pi3Crecti(leftbarWidth, botbarHeight, (int)((float)workWidth*dragBarX), (int)((float)workHeight*iby));
	if (views[BOTTOMLEFT].viewport.touch(mx, my)) currentView = BOTTOMLEFT;
	renderScene(views[BOTTOMLEFT]);

	//render left top ...
	views[TOPLEFT].viewport = Pi3Crecti(leftbarWidth, botbarHeight + (int)((float)workHeight * iby), (int)((float)workWidth*dragBarX), (int)((float)workHeight*dragBarY));
	if (views[TOPLEFT].viewport.touch(mx, my)) currentView = TOPLEFT;
	renderScene(views[TOPLEFT]);

	//render right top ...
	views[TOPRIGHT].viewport = Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX), botbarHeight + (int)((float)workHeight * iby), (int)((float)workWidth*ibx), (int)((float)workHeight*dragBarY));
	if (views[TOPRIGHT].viewport.touch(mx, my)) currentView = TOPRIGHT;
	renderScene(views[TOPRIGHT]);

	//Render 2D
	scene.setViewport(Pi3Crecti(0, 0, hw, hh));
	scene.setFixedLight(0xffffff, vec3f(0, 1000.f, 1000.f));
	scene.setViewport2D(Pi3Crecti(0, 0, hw, hh), 0.1f, 2000.f);
	scene.render2D(window->getTicks());

	handleIMGui(); //must be in the rendering loop with 2D setup

}

//struct modelInfo {
//	std::string group;
//	std::string name;
//	std::string collider;
//	bool gridLocked;
//	std::vector<std::string> LODmodels;
//	std::vector<float> LODdists;
//};
//
//modelInfo getModelInfo(const std::string &str)
//{
//	modelInfo lodmodels;
//
//	std::stringstream ss(str);
//	std::string model, gl;
//	float lodVal = 0;
//	ss >> lodmodels.group >> gl >> lodmodels.name >> model >> lodmodels.collider >> lodVal;
//
//	lodmodels.gridLocked = (gl == "g");
//	lodmodels.LODmodels.push_back(model);
//	lodmodels.LODdists.push_back(lodVal);
//
//	while (ss >> model) {
//		ss >> lodVal;
//		lodmodels.LODmodels.push_back(model);
//		lodmodels.LODdists.push_back(lodVal);
//	}
//	return lodmodels;
//}
//
//void Modeller::loadModelLibrary(const std::string &path, const std::vector<std::string> &vals)
//{
//	//loadOptions opts(file.c_str());
//
//	Pi3Cmodel::modelParams mp;
//	mp.resource = resource;
//	mp.path = path;
//	Pi3Cmodel *models = nullptr;
//
//	for (auto &val : vals) {
//		modelInfo minfo = getModelInfo(val);
//
//		auto findLib = modelLibrary.find(minfo.group);
//		if (findLib == modelLibrary.end()) {
//			modelLibrary.insert({ minfo.group, Pi3Cmodel()});
//			auto findLib2 = modelLibrary.find(minfo.group);
//			if (findLib2 != modelLibrary.end()) models = &findLib2->second; else models = nullptr; //somethings seriously wrong!
//		}
//		else {
//			models = &findLib->second;
//		}
//
//		mp.name = minfo.name;
//		Pi3Cmodel model(mp);
//
//		float lodfrom = 0.f;
//		for (size_t j = 0; j < minfo.LODmodels.size(); j++) {
//			std::string colliderFile = (j == 0) ? minfo.collider : "";
//			std::string modelFile = minfo.LODmodels[j];
//			Pi3Cmodel modelLOD(resource, modelFile, mp.path, modelFile, colliderFile);
//			float lodToo = minfo.LODdists[j];
//			model.appendLOD(modelLOD, lodfrom, lodToo);
//			lodfrom = lodToo;
//		}
//		models->append(model);
//	}
//}
//
//void Modeller::open()
//{
//	if (scene.models.size()>0) scene.models[0].group.clear();
//	Pi3Cmodel sceneModel = loadScene("CastleScene.txt", grid);
//	scene.models[0] = sceneModel;
//	// sceneModelRef = scene.append3D(sceneModel);
//}
//
//Pi3Cmodel Modeller::loadScene(const std::string &file, vec3f &grid)
//{
//	loadOptions opts(file.c_str());
//
//	Pi3Cmodel model;
//	model.name = opts.asString("title");
//	grid = opts.asVec3f("gridsize");
//	std::vector<std::string> objects = opts.asStringArray("object");
//
//	std::string objname;
//	float gx, gy, gz, rot;
//	for (auto &o : objects) {
//		std::stringstream ss(o);
//		ss >> objname >> gx >> gy >> gz >> rot;
//		Pi3Cmodel *fmodel = findModel(objname);
//		if (fmodel) {
//			vec3f position = { gx*grid.x, gy*grid.y, gz*grid.z };
//			vec3f rotation = { 0, rot *DEG2RAD, 0 };
//			model.append(*fmodel, position, rotation);
//		}
//	}
//	return model;
//}
//
//void Modeller::saveScene(const std::string &file, Pi3Cmodel *models)
//{
//	loadOptions opts(file.c_str());
//
//	for (auto &model : models->group) {
//		std::stringstream ss;
//		vec3f position = model.matrix.position();
//		vec3f rot = model.matrix.getRotation() * RAD2DEG;
//		ss << model.name << position.x << position.y << position.z << rot.x << rot.y << rot.z;
//	}
//}
