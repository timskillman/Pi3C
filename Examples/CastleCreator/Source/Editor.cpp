#include "Editor.h"
#include "Pi3Cshapes.h"
#include "Pi3Crect.h"
#include "Pi3Cdirectory.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>


//using namespace rapidjson;

Editor::Editor(Pi3Cresource *resource, Pi3Cwindow *window)
{
	// Setup shader ..
	int32_t basicShaderRef = resource->addShader("assets/shaders/vs1.glsl", "assets/shaders/fs1.glsl");
	if (basicShaderRef < 0) {
		window->showErrorMessage("Shader Error!", resource->error());
		return;
	}

	this->resource = resource;
	this->window = window;

	gridlock = true;
	
	gui.init(resource, window);

	std::vector<std::string> dir = Pi3Cdirectory::readDirectory("../");

	// Setup scene ...
	scene.resource = resource;
	scene.selectShader(basicShaderRef);
	scene.setViewport2D(Pi3Crecti(0, 0, (int32_t)window->getWidth(), (int32_t)window->getHeight()), 0.1f, 2000.f);

}

//Pi3Cmodel Editor::createScene(const uint32_t width, const uint32_t depth, Pi3Cmodel *usemodel, const vec3f &grid)
//{
//	float hw = width * grid.x * 0.5f;
//	float hd = depth * grid.z * 0.5f;
//	Pi3Cmodel model;
//	//Pi3Cmodel *findModel = modelsLib.find(modelname);
//	if (!usemodel) return model;
//
//	for (uint32_t z = 0; z < depth; z++)
//	{
//		for (uint32_t x = 0; x < width; x++)
//		{
//			vec3f position = { x*grid.x-hw, 0, z*grid.z-hd };
//			vec3f rotation = { 0, 0, 0 };
//			model.append(*usemodel, position, rotation);
//		}
//	}
//	return model;
//}

Pi3Cmodel * Editor::selectLib(const std::string &libName)
{
	for (auto &lib : modelLibrary) {
		if (lib.first == libName) return &lib.second;
	}
	return nullptr;
}

Pi3Cmodel * Editor::findModel(const std::string &modelName)
{
	for (auto &lib : modelLibrary) {
		Pi3Cmodel *model = lib.second.find(modelName);
		if (model) return model;
	}
	return nullptr;
}

void Editor::newScene(const uint32_t ref)
{
	Pi3Cmodel sceneModel = *findModel("Landscape"); // createScene(1, 1, findModel("Landscape"), grid);
	Pi3Cmodel &submod = sceneModel.group[0];
	submod.edit = false;
	if (scene.models.size() == 0) 
		scene.append3D(sceneModel); 
	else
		scene.models[0].group.push_back(sceneModel);
}

void Editor::setupGUI(std::string fontsPath, std::string fontName) // &opts)
{
	// Get GUI fonts
	//SDL_Log("Loading fonts: %s , %s", opts.asString("fontsPath").c_str(),opts.asString("font1").c_str());
	 
	//resource->addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 80);
	guifonts.push_back(gui.addFont(fontsPath.c_str(), fontName.c_str(), 22)); // opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 22));
	guifonts.push_back(gui.addFont(fontsPath.c_str(), fontName.c_str(), 16));
	//guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 16));
	
	//gui.loadImages("../../Resources/models/maps", { "larrow.png","rarrow.png","darrow.png","uarrow.png","handle.png","radioon.png","radiooff.png" });
	// Setup GUI button styles ...
	bsMenu.font = guifonts[1];
	bsMenu.textColour = 0xff303030;
	bsMenu.buttonColour = 0xffffffff;
	bsMenu.minHeight = 26;
	bsMenu.minWidth = 70;
	bsMenu.vertGap = 0;
	bsMenu.left = 10.f;
	bsMenu.highlightColour = 0xc0c0c0;

	bsHeading.font = guifonts[0];
	bsHeading.textColour = 0xff000000;
	bsHeading.highlightColour = 0xffffff;
	bsHeading.buttonColour = 0xffffffff;

	bsCategories.font = guifonts[0];
	bsCategories.buttonAlpha = 0.5f;
	bsCategories.textColour = 0xffffffff;
	bsCategories.selectColour = 0xff008080;
	bsCategories.sameLine = true;

	bsItems.font = guifonts[1];
	bsItems.buttonAlpha = 0.8f;
	bsItems.buttonColour = 0xffffffff;
	bsItems.textColour = 0xff404040;
	bsItems.minWidth = 200;
	bsItems.minHeight= 40;
	bsItems.justify = Pi3Cimgui::CENTRE;
	bsItems.align = Pi3Cimgui::BOTTOM;
	bsItems.selectColour = 0x808000;
}

void Editor::loadModels(const std::string &modelsLibraryFile)
{
	// Load 3D models and scene ...

	//Setup 2D interface ...
	setupGUI("../../Resources/fonts/", "NotoSans-Regular.ttf");

	// Load model library ...
	loadModelLibraryJSON(modelsLibraryFile);

	for (auto &ln : modelLibrary) libnames.push_back(ln.first);

	// Create a brush for touching objects ...
	Pi3Cmodel brush = Pi3Cmodel(resource, Pi3Cshapes::sphere(vec3f(0, 0, 0), 2.0f, 0.f, 10, 20), -1, 0xffff00);
	brush.touchable = false;
	brush.visible = false;
	brush.material.alpha = 0.5f;
	ballbrushref = scene.append3D(brush);

	Pi3Cmodel gridbrush = Pi3Cmodel(resource, Pi3Cshapes::cuboid(vec3f(0, 0, 0), vec3f(grid.x,1.f, grid.z), 1,1,1), -1, 0xffff00);
	gridbrush.touchable = false;
	gridbrush.visible = true;
	gridbrush.material.alpha = 0.5f;
	gridbrushref = scene.append3D(gridbrush);

	brushref = gridbrushref;

	// Create a humanoid ...
	//Pi3Chumanoid::humanoidParams bodyParams;
	//int32_t bodyRef = scene.append3D(body.create(resource, bodyParams));

	currentLib = libnames[0];
	modelsLib = selectLib(currentLib);
	currentModel = modelsLib->group[0].name;
	

	// Setup humanoid body pointers (can corrupt if done too early)
	//body.attachModel(scene.models[bodyRef]);

}

void Editor::handleKeys()
{
	player.moved = false;
	const uint8_t *keystate = window->getKeys();
	bool shiftKey = keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT];
	bool ctrlKey = keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL];
	
	player.run(shiftKey);
	if (keystate[SDL_SCANCODE_W]) if (editMode) editpos.y += 5.f; else player.forward();
	if (keystate[SDL_SCANCODE_S] && !ctrlKey) if (editMode) editpos.y -= 5.f; else player.back();
	if (keystate[SDL_SCANCODE_A]) if (editMode) editpos.x += 5.f; else player.left();
	if (keystate[SDL_SCANCODE_D]) if (editMode) editpos.x -= 5.f; else player.right();
	if (keystate[SDL_SCANCODE_R]) if (editMode) editpos.z += 5.f; else player.up();			//only when flying
	if (keystate[SDL_SCANCODE_F]) if (editMode) editpos.z -= 5.f; else player.down();		//only when flying
	if (keystate[SDL_SCANCODE_SPACE]) player.jump();	//only when walking
	if (keystate[SDL_SCANCODE_ESCAPE]) window->setquit(true);

	if (!keypress) {
		if (!modeChange && keystate[SDL_SCANCODE_V]) {
			editMode = !editMode;
			if (editMode) edTransition = 0.99f; else edTransition = 0.01f;
			SDL_Log("edTransition = %f", edTransition);
			modeChange = true;
			keypress = true;
		}
		if (keystate[SDL_SCANCODE_G]) { 
			gridlock = !gridlock; 
			keypress = true; 
		}
		if (keystate[SDL_SCANCODE_DELETE]) {
			if (selGroup) {
				Pi3Cmodel &touchMod = scene.models[touch.groupRefs[0]].group[touch.groupRefs[1]];
				if (touchMod.edit) touchMod.deleted = true;
			}
			keypress = true;
		}
		if (keystate[SDL_SCANCODE_S] && ctrlKey) {
			//std::string file = openFileSave("");
			save("MyCastleScene");
			keypress = true;
		}
	}
	else if (!window->keydown()) keypress = false;

	player.updateAndCollide(&scene, window->getTicks());
}

void Editor::handleEvents(std::vector<uint32_t>& eventList)
{
	if (eventList.size() == 0) return;

	// process window events ...
	for (auto& ev : eventList) {
		switch (ev)
		{
		case SDL_MOUSEMOTION:
			if (window->mouse.anyButton()) {
				if (window->mouse.RightButton) {
					vec3f rotateVec(window->mouse.deltaXY.y / 300.f, -window->mouse.deltaXY.x / 300.f, 0);
					if (editMode) 
						editrot += rotateVec;
					else
						player.rotate(rotateVec);
				}
			}
			break;
		case SDL_MOUSEWHEEL:
			//if (editMode) editpos.z += window->mouse.wheel * 20.f;
			break;
		case SDL_WINDOWEVENT:
			//switch (window->ev.window.event) {
			if (window->resized) {
				scene.setPerspective3D(window->getWidth(), window->getHeight(), 800.f, nearzfarz.x, nearzfarz.y);
				scene.setViewport2D(Pi3Crecti(0, 0, (int32_t)window->getWidth(), (int32_t)window->getHeight()), 0.1f, 2000.f);
				gui.resize();
				window->resized = false;
			}
			break;
		}
	}
}

vec3f Editor::getGridPos(vec3f gpos)
{
	if (gridlock && gpos.y < 1.f) 
		gpos = vec3f(floorf((gpos.x + grid.x*0.5f) / grid.x) * grid.x, 0, floorf((gpos.z + grid.z*0.5f) / grid.z) * grid.z);
	return gpos;
}

float randomRange(float a, float b)
{
	return a + ((float)(std::rand() % 10000) / 10000.f)* (b - a);
}

void Editor::touchScene()
{
	selGroup = nullptr;
	if (brushref>=0) scene.models[brushref].visible = false;

	if (!player.moved && !gui.somethingSelected) {

		vec3f mousexyz(window->getWidth()*0.5f - window->mouse.x, window->mouse.y - window->getHeight()*0.5f, 0);
		touch = scene.touch(mousexyz, true);

		if (touch.touched()) {

			//move 3D pointer (sphere) to touch intersetion point ...
			if (brushref >= 0) {
				scene.models[brushref].visible = false;
				vec3f gpos = getGridPos(touch.intersection);
				brushref = (gridlock && gpos.y<1.f) ? gridbrushref : ballbrushref;
				scene.models[brushref].matrix.move(gpos);
				scene.models[brushref].visible = true;
			}

			
			//SDL_Log("Touch tri = %d", touch.triRef/48);

			if (touch.maxlevel > 1) {

				if (window->mouse.LeftButton && window->mouse.up && currentItemSel<modelsLib->group.size()) {
					Pi3Cmodel newmod = modelsLib->group[currentItemSel];
					//if (gridlock) {
					//	//Replace selected object with library object
					//	Pi3Cmodel &touchMod = scene.models[touch.groupRefs[0]].group[touch.groupRefs[1]];
					//	newmod.matrix = touchMod.matrix;		//use old matrix of object to replace
					//	touchMod = newmod;
					//}
					//else {
						//Insert library object as touch position
						vec3f gpos = getGridPos(touch.intersection);
						newmod.matrix.move(gpos);
						newmod.matrix.SetScale(randomRange(newmod.randomScale.x, newmod.randomScale.y)); // +((float)(std::rand() % 1000) / 1000.f)* (newmod.randomScale.y - newmod.randomScale.x));
						if (newmod.randomRotation.x != 0 || newmod.randomRotation.y != 0) {
							Pi3Cmatrix rotmtx;
							float rotateBy = randomRange(newmod.randomRotation.x * DEG2RAD, newmod.randomRotation.y * DEG2RAD);
							rotmtx.SetRotateYbit(rotateBy);
							newmod.matrix = rotmtx * newmod.matrix;
							newmod.rotation.y += rotateBy;
						}
						scene.models[0].append(resource, newmod);
					//}
					window->mouse.up = false;
				}
				else if (window->mouse.wheel) {
					//Rotate selected object ...
					Pi3Cmodel *rotGroup = scene.getSubModel(&touch.groupRefs[0], 1);
					if (rotGroup->edit) {
						Pi3Cmatrix rotmtx;
						float sgn = (float)window->mouse.wheel;
						float rotateBy = (!gridlock) ? (PI / 8.f)*sgn : (PI / 2.f)*sgn;
						rotmtx.SetRotateYbit(rotateBy);
						rotGroup->matrix = rotmtx * rotGroup->matrix;
						rotGroup->rotation.y += rotateBy;
					}
					window->mouse.wheel = 0;
				}
				else if (!window->mouse.anyButton()) {
					selGroup = scene.getSubModel(&touch.groupRefs[0], 2);
					//SDL_Log("Intersect = %f,%f,%f, %s", touch.intersection.x, touch.intersection.y, touch.intersection.z, selGroup->name.c_str());

					lastCol = touch.selmodel->material.colAmbient;
					for (auto &m : selGroup->group) m.material.SetColAmbient(0xffffffff);
				}
			}
		}
	}
}

vec3f nearestAngles(vec3f rot)
{
	float rx = ((float)((int)((rot.x * RAD2DEG)*1000.f) % 360000) / 1000.f)*DEG2RAD;
	float ry = ((float)((int)((rot.y * RAD2DEG)*1000.f) % 360000) / 1000.f)*DEG2RAD;
	float rz = ((float)((int)((rot.z * RAD2DEG)*1000.f) % 360000) / 1000.f)*DEG2RAD;
	return vec3f(rx, ry, rx);
}

void Editor::tweens()
{
	if (edTransition > 0 && edTransition < 1.f) {
		currentPos = vec3f(editpos.x, 0, editpos.y)*(1.f - edTransition) + player.getPosition() * edTransition;
		current3rd = vec3f(0, 0, editpos.z)*(1.f - edTransition);
		currentRot = nearestAngles(editrot) * (1.f - edTransition) + nearestAngles(player.getRotation()) * edTransition;
		float speed = 0.01f*window->getTicks()*60.f;
		if (editMode) edTransition -= speed; else edTransition += speed;
	}
	else {
		if (editMode) {
			currentPos = vec3f(editpos.x, 0, editpos.y);
			current3rd = vec3f(0, 0, editpos.z);
			currentRot = editrot;
		}
		else {
			currentPos = player.getPosition();
			current3rd = vec3f(0, 0, 0);
			currentRot = player.getRotation();
		}
		modeChange = false;
	}
}

void Editor::animate()
{
	//body.walk(vec3f(0, 0, 0), 0.05f * window->getTicks());
}

void Editor::handleIMGui()
{
	gui.Begin();

	//Menubar ...
	gui.setButtonStyle(bsMenu);
	if (gui.BeginMenuBar("MainMenu")) {
		if (gui.BeginMenu("File")) {
			if (gui.MenuItem("New", "Ctrl+N")) newScene(sceneModelRef);
			if (gui.MenuItem("Open", "Ctrl+O")) open();
			if (gui.MenuItem("Save", "Ctrl+S")) save("MyCastleScene");
			if (gui.MenuItem("Quit", "")) window->setquit(true);
		}
		if (gui.BeginMenu("Edit")) {
			if (gui.MenuItem("Copy", "Ctrl+C")) {}
			if (gui.MenuItem("Paste", "Ctrl+V")) {}
			if (gui.MenuItem("Delete", "Del")) {}
			if (gui.MenuItem("Undo", "Ctrl+X")) {}
			if (gui.MenuItem("Redo", "Ctrl+Y")) {}
		}
		if (gui.BeginMenu("View")) {
			if (gui.MenuItem("Birds eye view", "")) {}
			if (gui.MenuItem("Fullscreen", "Ctrl+F")) {}
		}
		gui.EndMenuBar("MainMenu");
	}

	gui.movePosition(10, 3);

	//Category buttons ...
	gui.setButtonStyle(bsCategories);
	for (auto &libname : libnames) {
		if (gui.ButtonText(libname, libname == currentLib, (window->getWidth() - 32) / (float)(libnames.size()))) {
			currentLib = libname;
			modelsLib = selectLib(libname);
			currentModel = modelsLib->group[0].name;
			currentItemSel = 0;
			player.moved = true;
		}
	}

	//Item buttons ...
	gui.nextLine();
	gui.setButtonStyle(bsItems);
	for (size_t i = 0; i < modelsLib->group.size(); i++) {
		std::string &name = modelsLib->group[i].name;
		if (gui.ButtonText(name, currentModel == name)) {
			currentModel = name;
			currentItemSel = i;
			player.moved = true;
		}
	}
}

void Editor::render()
{
	static float rq = 0;
	resource->calls = 0;

	//Render 3D
	scene.setMatrix(currentPos, current3rd, currentRot);
	scene.setSun(0xffffff, vec3f(1000.f, 1000.f, -1000.f)); //transform sun position into scene
	scene.render3D(window->getTicks());

	//Render 2D
	scene.setFixedLight(0xffffff, vec3f(0, 1000.f, 1000.f));
	scene.render2D(window->getTicks());

	handleIMGui(); //must be in the rendering loop with 2D setup

	//scene.models[textRef].textModel(resource, gui.getFont(guifonts[0]).get(), livetext, 1000+sin(rq)*800.f);
	//rq = rq + 0.03f;

	if (selGroup) {
		for (auto &m : selGroup->group) m.material.colAmbient = lastCol;
	}
}

void Editor::loadModelLibraryJSON(const std::string &file)
{
	Pi3Cmodel *models = nullptr;

	Pi3Cjson json(file.c_str());

	if (!json.doc.IsObject()) return;

	std::string libraryName = json.readString(json.doc, "title");
	std::string modelPath = json.readString(json.doc, "modelPath");

	const Value& vw = json.doc["view"];
	int32_t scwidth, scheight;
	json.readInt2(vw, "screensize", scwidth, scheight);

	nearzfarz = json.readVec2f(vw, "nearzfarz");
	scene.setPerspective3D(scwidth, scheight, json.readFloat(vw, "perspective"), nearzfarz.x, nearzfarz.y);
	scene.setFog(json.readHex(vw, "fogColour"), json.readFloat(vw, "fogNear"), json.readFloat(vw, "fogFar"));
	grid = json.readVec3f(vw, "grid");

	std::string skyboxpath = json.readString(vw, "skyboxPath");
	std::string skyboxfile = json.readString(vw, "skybox");

	//std::string fontsPath = json.readString(vw, "fontsPath");
	//std::string fontName = json.readString(vw, "font");
	//setupGUI(fontsPath, fontName);

	const Value& av = json.doc["avatar"];
	Pi3Cavatar::avatarParams avparams;
	avparams.movement = json.readBool(av, "fly") ? Pi3Cavatar::move_fly : Pi3Cavatar::move_walk;
	avparams.position = json.readVec3f(av, "position");
	avparams.rotation = json.readVec3f(av, "rotation");
	avparams.size = { 1.f, json.readFloat(av,"height"), 1.f };
	avparams.fallSpeed = json.readFloat(av, "fallSpeed");
	avparams.walkSpeed = json.readFloat(av, "walkSpeed");
	avparams.runSpeed = json.readFloat(av, "runSpeed");
	player.init(avparams);

	const Value& obj = json.doc["models"];
	if (obj.IsArray()) {
		for (SizeType i = 0; i < obj.Size(); i++) {
			if (obj[i].IsObject()) {
				//modelInfo lodmodels;

				Pi3Cmodel model;

				const Value& ob = obj[i];
				std::string category = json.readString(ob, "category");
				std::string modelname = json.readString(ob, "name");
				std::string modelID = json.readString(ob, "id");
				bool gridLocked = (json.readString(ob, "flags") == "g");
				std::string collider = json.readString(ob, "colliderFile");
				vec2f randomScale = { 1.f,1.f };
				vec2f randomRotation = { 0,0 };

				std::vector<std::string> LODmodels;
				std::vector<float> LODdists;
				if (ob.HasMember("LOD0")) {
					const Value& lod0 = ob["LOD0"];
					if (!lod0.IsNull()) {
						LODmodels.push_back(json.readString(lod0, "file"));
						LODdists.push_back(json.readFloat(lod0, "dist"));
					}
				}

				if (ob.HasMember("LOD1")) {
					const Value& lod1 = ob["LOD1"];
					if (!lod1.IsNull()) {
						LODmodels.push_back(json.readString(lod1, "file"));
						LODdists.push_back(json.readFloat(lod1, "dist"));
					}
				}

				if (ob.HasMember("RandomScale")) model.randomScale = json.readVec2f(ob, "RandomScale");
				if (ob.HasMember("RandomRotation")) model.randomRotation = json.readVec2f(ob, "RandomRotation");

				auto findLib = modelLibrary.find(category);
				if (findLib == modelLibrary.end()) {
					modelLibrary.insert({ category, Pi3Cmodel() });
					auto findLib2 = modelLibrary.find(category);
					if (findLib2 != modelLibrary.end()) models = &findLib2->second; else models = nullptr; //something's seriously wrong!
				}
				else {
					models = &findLib->second;
				}

				model.name = modelID;
				//model.desc = modelname;
				float lodfrom = 0.f;
				for (size_t j = 0; j < LODmodels.size(); j++) {
					std::string colliderFile = (j == 0) ? collider : "";
					std::string modelName = LODmodels[j];
					Pi3Cmodel modelLOD(resource, modelName, modelPath + "/" + category, modelName, 1, colliderFile);
					float lodToo = LODdists[j];
					model.appendLOD(resource, modelLOD, lodfrom, lodToo);
					lodfrom = lodToo;
				}
				models->append(resource, model);
			}
		}
	}

	newScene(0);

	skybox = scene.loadModelOBJ(skyboxpath, skyboxfile, vec3f(0,0,0), true, 0);
	scene.models[skybox].matrix.SetScale(json.readFloat(vw, "skyboxScale"));
	scene.models[skybox].touchable = false;

}

void Editor::open()
{
	if (scene.models.size()>0) newScene(0); //scene.models[0].group.clear();
	Pi3Cmodel sceneModel = loadSceneJSON("MyCastleScene.json", grid);
	for (auto& model : sceneModel.group) {
		scene.models[0].group.push_back(model);
	}
	//scene.models.push_back(sceneModel);
	//scene.models[0] = sceneModel;
}

void Editor::save(const std::string &file)
{
	saveSceneJSON(file, &scene.models[0]);
}

Pi3Cmodel Editor::loadSceneJSON(const std::string &file, vec3f &grid)
{
	Pi3Cjson json(file.c_str());
	
	Pi3Cmodel model;
	Pi3Cmatrix matrix;
	std::string objname;

	if (!json.doc.IsObject()) return model;
	model.name = json.readString(json.doc, "title");
	
	grid = json.readVec3f(json.doc, "grid");
	const Value& p = json.doc["player"];
	if (p.HasMember("position")) player.setPosition(json.readVec3f(p, "position"));
	if (p.HasMember("rotation")) player.setRotation(json.readVec3f(p, "rotation"));

	const Value& obj = json.doc["objects"];
	if (obj.IsArray()) {
		for (SizeType i = 0; i < obj.Size(); i++) {
			if (obj[i].IsObject()) {
				const Value& ob = obj[i];
				if (ob.HasMember("name")) objname = ob["name"].GetString();
				if (ob.HasMember("matrix")) matrix = json.readMatrix(ob, "matrix");
				Pi3Cmodel* m = findModel(objname);
				if (m) {
					Pi3Cmodel tmod = *m;
					tmod.matrix = matrix;
					model.append(resource, tmod);
				}
			}
		}
	}

	return model;
}

void Editor::saveSceneJSON(const std::string &file, Pi3Cmodel *models)
{
	vec3f ppos = player.getPosition();
	vec3f prot = player.getRotation();;

	Pi3Cjson json;

	json.StartObject();		//<<<There must be a root object surrounding everything

		json.writeString("title", file.c_str());
		json.writeVec3f("grid", grid);

		json.Key("player");
		json.StartObject();
			json.writeVec3f("position", ppos);
			json.writeVec3f("rotation", prot);
		json.EndObject();

		json.Key("objects");
		json.StartArray();
		for (auto &model : models->group) {
			if (!model.deleted) {
				json.StartObject();
				json.writeString("name", model.name.c_str());
				json.writeMatrix("matrix", model.matrix);
				json.EndObject();
			}
		}
		json.EndArray();

	json.EndObject();

	json.saveFile((file + ".json").c_str());

}

bool Editor::fileExists(const std::string &file)
{
	struct stat buffer;
	return (stat (file.c_str(), &buffer) == 0);
}
