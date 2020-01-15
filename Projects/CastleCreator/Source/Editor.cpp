#include "Editor.h"

//JSON
#include "rapidjson.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include "document.h"
#include <istreamwrapper.h>
#include <iostream>

#include "Pi3Cshapes.h"
#include "Pi3Crect.h"
#include "Pi3Cdirectory.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>


using namespace rapidjson;

Editor::Editor(Pi3Cresource *resource, Pi3Cwindow *window)
{
	// Setup shader ..
	int32_t basicShaderRef = resource->addShader("Shaders/vs1.glsl", "Shaders/fs1.glsl");
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
	scene.setViewport2D(Pi3Crecti(0, 0, (float)window->getWidth(), (float)window->getHeight()), 0.1f, 2000.f);

}

Pi3Cmodel Editor::createScene(const uint32_t width, const uint32_t depth, Pi3Cmodel *usemodel, const vec3f &grid)
{
	float hw = width * grid.x * 0.5f;
	float hd = depth * grid.z * 0.5f;
	Pi3Cmodel model;
	//Pi3Cmodel *findModel = modelsLib.find(modelname);
	if (!usemodel) return model;

	for (uint32_t z = 0; z < depth; z++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			vec3f position = { x*grid.x-hw, 0, z*grid.z-hd };
			vec3f rotation = { 0, 0, 0 };
			model.append(*usemodel, position, rotation);
		}
	}
	return model;
}

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
	Pi3Cmodel sceneModel = createScene(1, 1, findModel("Landscape"), grid);
	grid = vec3f(76.f, 0, 76.f);
	if (scene.models.size() > 0)
		scene.models[0] = sceneModel;
	else
		scene.models.push_back(sceneModel);
}

void Editor::setupGUI(loadOptions &opts)
{
	// Get GUI fonts
	SDL_Log("Loading fonts: %s , %s", opts.asString("fontsPath").c_str(),opts.asString("font1").c_str());
	 
	//resource->addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 80);
	guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 22));
	guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 16));
	
	//gui.loadImages("../../Resources/models/maps", { "larrow.png","rarrow.png","darrow.png","uarrow.png","handle.png","radioon.png","radiooff.png" });
	// Setup GUI button styles ...
	bsMenu.font = guifonts[1];
	bsMenu.textColour = 0xff303030;
	bsMenu.buttonColour = 0xffffffff;
	bsMenu.minHeight = 26.f;
	bsMenu.minWidth = 70.f;
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
	bsItems.minWidth = 140.f;
	bsItems.minHeight= 40.f;
	bsItems.justify = Pi3Cimgui::CENTRE;
	bsItems.align = Pi3Cimgui::BOTTOM;
	bsItems.selectColour = 0x808000;
}

void Editor::loadModels(const std::string &modelsLibraryFile, const std::string &scenefile)
{
	// Load 3D models and scene ...
	loadOptions opts(modelsLibraryFile.c_str());
	
	setupGUI(opts);

	nearzfarz = opts.asVec2f("nearzfarz");
	scene.setFog(0xffffff, 40000.f, 50000.f);
	scene.setPerspective3D(window->getWidth(), window->getHeight(), opts.asFloat("perspective"), nearzfarz.x, nearzfarz.y);

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

	// Load model library ...
	loadModelLibrary(opts.asString("modelPath"), opts.asStringArray("model"));
	for (auto &ln : modelLibrary) libnames.push_back(ln.first);

	grid = { 76.f ,0 ,76.f };
	newScene(0);
	Pi3Cmodel sceneModel = createScene(20, 20, findModel("Grass"), grid);

	// Load scene model ...
	//Pi3Cmodel sceneModel = loadScene(scenefile, &modelsLib, grid);
	//sceneModelRef = scene.append3D(sceneModel);

	// Load Skybox ...
	skybox = scene.loadModelOBJ(opts.asString("skyboxPath"), opts.asString("skybox"), 0); // loadbarCallback);
	scene.models[skybox].matrix.SetScale(opts.asFloat("skyboxScale"));
	scene.models[skybox].touchable = false;

	// Create a brush for touching objects ...
	Pi3Cmodel brush = Pi3Cmodel(resource, Pi3Cshapes::sphere(vec3f(0, 0, 0), 2.0f, 0xffffff, 0.f, 10, 20), 0xffff00);
	brush.touchable = false;
	brush.visible = false;
	brush.material.alpha = 0.5f;
	ballbrushref = scene.append3D(brush);

	Pi3Cmodel gridbrush = Pi3Cmodel(resource, Pi3Cshapes::cuboid(vec3f(0, 0, 0), vec3f(grid.x,1.f, grid.z), 0xffffff, 1,1,1), 0xffff00);
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
	//Setup 2D interface ...

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
				touchMod.deleted = true;
			}
			keypress = true;
		}
		if (keystate[SDL_SCANCODE_S] && ctrlKey) {
			save();
			keypress = true;
		}
	}
	else if (!window->keydown()) keypress = false;

	player.updateAndCollide(&scene, window->getTicks());
}

void Editor::handleEvents()
{
	// process window events ...
	while (window->event()) {
		switch (window->ev.type)
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
			//editpos.z += window->mouse.wheel * 20.f;
			break;
		case SDL_WINDOWEVENT:
			switch (window->ev.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				scene.setPerspective3D(window->getWidth(), window->getHeight(), 800.f, nearzfarz.x, nearzfarz.y);
				scene.setViewport2D(Pi3Crecti(0, 0, (float)window->getWidth(), (float)window->getHeight()), 0.1f, 2000.f);
				break;
			}
			break;
		}
	}
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
				vec3f gpos = touch.intersection;
				brushref = ballbrushref;
				if (gridlock && gpos.y < 1.f) {
					gpos = vec3f(floorf((gpos.x + grid.x*0.5f) / grid.x) * grid.x, 0, floorf((gpos.z + grid.z*0.5f) / grid.z) * grid.z);
					brushref = gridbrushref;
				}
				scene.models[brushref].matrix.move(gpos);
				scene.models[brushref].visible = true;
			}

			
			//SDL_Log("Touch tri = %d", touch.triRef/48);

			if (touch.maxlevel > 1) {

				if (window->mouse.LeftButton && window->mouse.up && currentSel<modelsLib->group.size()) {
					Pi3Cmodel newmod = modelsLib->group[currentSel];
					//if (gridlock) {
					//	//Replace selected object with library object
					//	Pi3Cmodel &touchMod = scene.models[touch.groupRefs[0]].group[touch.groupRefs[1]];
					//	newmod.matrix = touchMod.matrix;		//use old matrix of object to replace
					//	touchMod = newmod;
					//}
					//else {
						//Insert library object as touch position
						vec3f gpos = touch.intersection;
						if (gridlock) gpos = vec3f(floorf((gpos.x + grid.x*0.5f) / grid.x) * grid.x, gpos.y, floorf((gpos.z + grid.z*0.5f) / grid.z) * grid.z);
						newmod.matrix.move(gpos);
						scene.models[0].append(newmod);
					//}
					window->mouse.up = false;
				}
				else if (window->mouse.wheel) {
					//Rotate selected object ...
					Pi3Cmodel *rotGroup = scene.getSubModel(&touch.groupRefs[0], 1);
					Pi3Cmatrix rotmtx;
					float sgn = (float)window->mouse.wheel;
					float rotateBy = (!gridlock) ? (PI / 8.f)*sgn : (PI / 2.f)*sgn;
					rotmtx.SetRotateYbit(rotateBy);
					rotGroup->matrix = rotmtx * rotGroup->matrix;
					rotGroup->rotation.y += rotateBy;
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

void Editor::tweens()
{
	if (edTransition > 0 && edTransition < 1.f) {
		currentPos = vec3f(editpos.x, 0, editpos.y)*(1.f - edTransition) + player.getPosition() * edTransition;
		current3rd = vec3f(0, 0, editpos.z)*(1.f - edTransition);
		currentRot = editrot * (1.f - edTransition) + player.getRotation() * edTransition;
		float speed = 0.01f*window->getTicks();
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
	if (gui.BeginMenuBar()) {
		if (gui.BeginMenu("File")) {
			if (gui.MenuItem("New", "Ctrl+N")) newScene(sceneModelRef);
			if (gui.MenuItem("Open", "Ctrl+O")) open();
			if (gui.MenuItem("Save", "Ctrl+S")) save();
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
		gui.EndMenuBar();
	}

	gui.movePosition(10.f, 3.f);

	//Category buttons ...
	gui.setButtonStyle(bsCategories);
	for (auto &libname : libnames) {
		if (gui.ButtonText(libname, libname == currentLib, (window->getWidth() - 32.f) / (float)(libnames.size()))) {
			currentLib = libname;
			modelsLib = selectLib(libname);
			currentModel = modelsLib->group[0].name;
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
			currentSel = i;
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

struct modelInfo {
	std::string group;
	std::string name;
	std::string collider;
	bool gridLocked;
	std::vector<std::string> LODmodels;
	std::vector<float> LODdists;
};

modelInfo getModelInfo(const std::string &str)
{
	modelInfo lodmodels;

	std::stringstream ss(str);
	std::string model, gl;
	float lodVal = 0;
	ss >> lodmodels.group >> gl >> lodmodels.name >> model >> lodmodels.collider >> lodVal;

	lodmodels.gridLocked = (gl == "g");
	lodmodels.LODmodels.push_back(model);
	lodmodels.LODdists.push_back(lodVal);

	while (ss >> model) {
		ss >> lodVal;
		lodmodels.LODmodels.push_back(model);
		lodmodels.LODdists.push_back(lodVal);
	}
	return lodmodels;
}

void Editor::loadModelLibrary(const std::string &path, const std::vector<std::string> &vals)
{
	//loadOptions opts(file.c_str());

	//Pi3Cmodel::modelParams mp;
	//mp.resource = resource;
	//mp.path = path;
	Pi3Cmodel *models = nullptr;

	for (auto &val : vals) {
		modelInfo minfo = getModelInfo(val);

		auto findLib = modelLibrary.find(minfo.group);
		if (findLib == modelLibrary.end()) {
			modelLibrary.insert({ minfo.group, Pi3Cmodel()});
			auto findLib2 = modelLibrary.find(minfo.group);
			if (findLib2 != modelLibrary.end()) models = &findLib2->second; else models = nullptr; //something's seriously wrong!
		}
		else {
			models = &findLib->second;
		}

		//mp.name = minfo.name;
		Pi3Cmodel model; // (mp);
		model.name = minfo.name;

		float lodfrom = 0.f;
		for (size_t j = 0; j < minfo.LODmodels.size(); j++) {
			std::string colliderFile = (j == 0) ? minfo.collider : "";
			std::string modelFile = minfo.LODmodels[j];
			Pi3Cmodel modelLOD(resource, modelFile, path, modelFile, colliderFile);
			float lodToo = minfo.LODdists[j];
			model.appendLOD(modelLOD, lodfrom, lodToo);
			lodfrom = lodToo;
		}
		models->append(model);
	}
}

void Editor::open()
{
	if (scene.models.size()>0) scene.models[0].group.clear();
	//Pi3Cmodel sceneModel = loadScene("MyCastleScene.txt", grid);
	Pi3Cmodel sceneModel = loadSceneJSON("MyCastleScene.json", grid);
	scene.models[0] = sceneModel;
}

void Editor::save()
{
	saveScene("MyCastleScene", &scene.models[0]);
}

Pi3Cmodel Editor::loadScene(const std::string &file, vec3f &grid)
{
	loadOptions opts(file.c_str());

	Pi3Cmodel model;
	model.name = opts.asString("title");
	grid = opts.asVec3f("gridsize");
	std::vector<std::string> objects = opts.asStringArray("object");

	vec3f ppos = opts.asVec3f("playerPos");
	player.setPosition(ppos);
	vec3f prot = opts.asVec3f("playerRot");
	player.setRotation(prot);

	std::string objname;
	float gx, gy, gz;
	float rotx = 0, roty = 0, rotz = 0;

	for (auto &o : objects) {
		std::stringstream ss(o);
		ss >> objname >> gx >> gy >> gz >> rotx >> roty >> rotz;
		Pi3Cmodel *fmodel = findModel(objname);
		if (fmodel) {
			vec3f position =  { gx, gy, gz }; //{ gx*grid.x, gy*grid.y, gz*grid.z };
			vec3f rotation = { rotx *DEG2RAD, roty *DEG2RAD, rotz *DEG2RAD };
			model.append(*fmodel, position, rotation);
		}
	}
	return model;
}

vec3f readJSvec3f(const Value &d, const char * key)
{
	if (!d.HasMember(key)) return vec3f(0, 0, 0);
	const Value& a = d[key];
	if (a.Size() != 3) return vec3f(0, 0, 0);
	return vec3f((float)a[0].GetDouble(), (float)a[1].GetDouble(), (float)a[2].GetDouble());
}

Pi3Cmatrix readJSmatrix(const Value &d, const char * key)
{
	Pi3Cmatrix matrix;
	if (!d.HasMember(key)) return matrix;
	const Value& a = d[key];
	if (a.Size() != 12) return matrix;
	matrix.set(a[0].GetDouble(), a[1].GetDouble(), a[2].GetDouble(), 0,
		a[3].GetDouble(), a[4].GetDouble(), a[5].GetDouble(), 0, 
		a[6].GetDouble(), a[7].GetDouble(), a[8].GetDouble(), 0, 
		a[9].GetDouble(), a[10].GetDouble(), a[11].GetDouble(), 1.f);
	return matrix;
}

Pi3Cmodel Editor::loadSceneJSON(const std::string &file, vec3f &grid)
{
	std::ifstream ifs(file.c_str());
	IStreamWrapper isw(ifs);

	Document d;
	d.ParseStream(isw);
	
	Pi3Cmodel model;
	Pi3Cmatrix matrix;
	std::string objname;

	if (!d.IsObject()) return model;

	if (d.HasMember("title")) model.name = d["title"].GetString();
	
	grid = readJSvec3f(d, "grid");
	const Value& p = d["player"];
	if (p.HasMember("position")) player.setPosition(readJSvec3f(p, "position"));
	if (p.HasMember("rotation")) player.setRotation(readJSvec3f(p, "rotation"));

	const Value& obj = d["objects"];
	if (obj.IsArray()) {
		for (SizeType i = 0; i < obj.Size(); i++) {
			if (obj[i].IsObject()) {
				const Value& ob = obj[i];
				if (ob.HasMember("name")) objname = ob["name"].GetString();
				if (ob.HasMember("matrix")) matrix = readJSmatrix(ob, "matrix");
				model.append(*findModel(objname), matrix);
			}
		}
	}

	return model;
}

double formatDbl(float v)
{
	return std::floor((double)v * 10000) / 10000;
}

void writeJSstring(PrettyWriter<StringBuffer> &json_writer, const char * key, const char * str)
{
	json_writer.Key(key);
	json_writer.String(str);
}

void writeJSvec3f(PrettyWriter<StringBuffer> &json_writer, const char * key, vec3f& v)
{
	json_writer.Key(key);
	json_writer.StartArray();
	json_writer.Double(formatDbl(v.x));
	json_writer.Double(formatDbl(v.y));
	json_writer.Double(formatDbl(v.z));
	json_writer.EndArray();
}

void writeJSmatrix(PrettyWriter<StringBuffer> &json_writer, const char * key, Pi3Cmatrix& matrix)
{
	json_writer.Key(key);
	json_writer.StartArray();
	const float* mv = matrix.get();
	json_writer.Double(formatDbl(mv[matrix.m00]));
	json_writer.Double(formatDbl(mv[matrix.m01]));
	json_writer.Double(formatDbl(mv[matrix.m02]));
	json_writer.Double(formatDbl(mv[matrix.m10]));
	json_writer.Double(formatDbl(mv[matrix.m11]));
	json_writer.Double(formatDbl(mv[matrix.m12]));
	json_writer.Double(formatDbl(mv[matrix.m20]));
	json_writer.Double(formatDbl(mv[matrix.m21]));
	json_writer.Double(formatDbl(mv[matrix.m22]));
	json_writer.Double(formatDbl(mv[matrix.m30]));
	json_writer.Double(formatDbl(mv[matrix.m31]));
	json_writer.Double(formatDbl(mv[matrix.m32]));
	json_writer.EndArray();
}

void Editor::saveScene(const std::string &file, Pi3Cmodel *models)
{
	//loadOptions opts(file.c_str());
	vec3f ppos = player.getPosition();
	vec3f prot = player.getRotation();

	//std::ofstream ofs(file + ".txt", std::ofstream::out);
	//ofs << "title:CastleScene\n";
	//ofs << "gridsize:" << grid.x << " " << grid.y << " " << grid.z << "\n";
	//ofs << "playerPos:" << ppos.x << " " << ppos.y << " " << ppos.z << "\n";
	//ofs << "playerRot:" << prot.x << " " << prot.y << " " << prot.z << "\n";
	//for (auto &model : models->group) {
	//	if (!model.deleted) {
	//		vec3f position = model.matrix.position();
	//		vec3f rot = model.rotation * RAD2DEG; // matrix.getRotation() * RAD2DEG;
	//		ofs << "object:" << model.name << " " << position.x << " " << position.y << " " << position.z << " " << rot.x << " " << rot.y << " " << rot.z << "\n";
	//	}
	//}
	//ofs.close();

	StringBuffer json_string_buffer;
	PrettyWriter<StringBuffer> json_writer(json_string_buffer);
	json_writer.SetFormatOptions(kFormatSingleLineArray);

	json_writer.StartObject();		//<<<There must be a root object surrounding everything

		writeJSstring(json_writer, "title", file.c_str());
		writeJSvec3f(json_writer, "grid", grid);

		json_writer.Key("player");
		json_writer.StartObject();
			writeJSvec3f(json_writer, "position", ppos);
			writeJSvec3f(json_writer, "rotation", prot);
		json_writer.EndObject();

		json_writer.Key("objects");
		json_writer.StartArray();
		for (auto &model : models->group) {
			if (!model.deleted) {
				json_writer.StartObject();
				writeJSstring(json_writer, "name", model.name.c_str());
				writeJSmatrix(json_writer, "matrix", model.matrix);
				json_writer.EndObject();
			}
		}
		json_writer.EndArray();

	json_writer.EndObject();

	std::ofstream jsonfs(file + ".json", std::ofstream::out);
	jsonfs << json_string_buffer.GetString() << std::endl;
	jsonfs.close();

}

bool Editor::fileExists(const std::string &file)
{
	struct stat buffer;
	return (stat (file.c_str(), &buffer) == 0);
}
