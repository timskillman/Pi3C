#include "Pi3C.h"

Pi3Cwindow Pi3C::window;
Pi3Cresource Pi3C::resource;

#define SYSTEM_GPID -1
#define MODEL_GPID 1

Pi3C::Pi3C()
{
}

Pi3C::~Pi3C()
{
	window.destroy();
}

Pi3C::Pi3C(const Pi3Cwindow::options& winopts)
{
	init(winopts);
}

Pi3C::Pi3C(const std::string title, const uint32_t width, const uint32_t height, const bool fullscreen) 
{
	newWindow(title, width, height, fullscreen);
}

void Pi3C::newWindow(const std::string &title, const uint32_t width, const uint32_t height, const bool fullscreen)
{
	Pi3Cwindow::options winopts;
	winopts.title = title;
	if (width > 0 && height > 0) {
		winopts.width = width;
		winopts.height = height;
		winopts.fullscreen = fullscreen;
	}
	init(winopts);
}

void Pi3C::init(const Pi3Cwindow::options& winopts)
{
	window.initOptions(winopts);
	
	winw = window.getWidth();
	winh = window.getHeight();

	resource.init(); //resource.init must be called after window creation

	// Setup shader ..
	int32_t basicShaderRef = resource.addShader("assets/shaders/vs1.glsl", "assets/shaders/fs1.glsl");
	if (basicShaderRef < 0) {
		window.showErrorMessage("Shader Error!", resource.error());
		return;
	}

	scene.init(&resource);
	scene.selectShader(basicShaderRef);

	scene.setFog(0xffffff, 10000.f, 25000.f);
	scene.setViewport2D(Pi3Crecti(0, 0, winw, winh), winopts.nearZ, winopts.farZ);
	scene.setPerspective3D(winw, winh, winopts.perspective, winopts.nearZ, winopts.farZ);

	gui.init(&resource, &window);
	guifonts.push_back(gui.addFont("../../Resources/fonts/", "NotoSans-Regular.ttf", 18));

	bsMenu.font = guifonts[0];
	bsMenu.textColour = 0x202020;
	bsMenu.buttonAlpha = 0.99f;
	bsMenu.buttonColour = 0xffffff;
	bsMenu.minWidth = 60;
	bsMenu.minHeight = 24;
	bsMenu.top = 3;
	bsMenu.bottom = 3;
	bsMenu.vertGap = 0;
	bsMenu.left = 10;
	bsMenu.highlightColour = 0xc0c0c0;
}


Pi3Cmodel Pi3C::create_model_from_text(const std::string &text, const uint32_t width, const uint32_t colour)
{
	Pi3Cmodel textModel;
	textModel.textModel(&resource, gui.getFont(guifonts[0]).get(), text, (float)width, Pi3Cfont::RD_HTML);
	textModel.material.SetColDiffuse(colour);
	return textModel;
}

void Pi3C::addColliderGridToModel(uint32_t modelRef)
{
	Pi3Cmodel& model = scene.models[modelRef];
	model.addColliderGrid(&resource);
	model.asCollider = true;
}

uint32_t Pi3C::add_background(const std::string &path, const std::string &file)
{
	Pi3Cmodel background;

	Pi3Cmesh rect;
	rect.addRect(vec3f(0, (float)winh, -20.f), vec2f((float)winw, (float)winh));
	//background.meshRef = resource.addMesh(rect);
	background.meshRef = resource.addMesh(&rect, 1);

	background.addTexture(&resource, (file!="" && path!="") ? path+"/"+file : path);
	background.material.illum = 1;
	background.material.alpha = 1.f;
	background.visible = false;

	int bkgmodelRef = add_model_to_scene2D(background);
	if (bkgmodelRef >= 0) {
		currentBackground = backgrounds.size();
		backgrounds.push_back(bkgmodelRef);
	}
	return currentBackground;
}

void Pi3C::showBackground(const int backgroundRef) {
	scene.models2D[backgrounds[currentBackground % backgrounds.size()]].visible = false;
	currentBackground = backgroundRef;
	scene.models2D[backgrounds[currentBackground % backgrounds.size()]].visible = true;
};

void Pi3C::nextBackground() {
	showBackground((currentBackground + 1) % backgrounds.size());
}

//void Pi3C::update_sprite_position(const uint32_t spritesRef, const uint32_t spriteRef, const float x, const float y)
//{
//	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
//	rm->updateSpriteCoords(resource.vertBuffer[rm->bufRef], spriteRef, x, y);
//}
//
//void Pi3C::update_sprite_rotated(const uint32_t spritesRef, const uint32_t spriteRef, const vec2f &pos, const vec2f &size, const float angle)
//{
//	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
//	rm->updateSpriteCoordsRotated(resource.vertBuffer[rm->bufRef], spriteRef, pos, size, angle);
//}
//
//void Pi3C::update_sprite_transform(const uint32_t spritesRef, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent)
//{
//	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
//	rm->updateSpriteTransformCoords(resource.vertBuffer[rm->bufRef], spriteRef, pos, size, scene_matrix, cent);
//}
//
//void Pi3C::update_sprite_billboards(const uint32_t spritesRef, const uint32_t spriteRef, const uint32_t count, const vec3f &lookat)
//{
//	Pi3CspriteArray *sa = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
//	uint32_t p = sa->calcRectPtr(0);
//	std::vector<float> &buf = resource.vertBuffer[sa->bufRef];
//	rm
//	for (uint32_t s = 0; s < count; s++) {
//		rm->updateSpriteBillboard(buf, p, rm->sprite_pos[s], rm->sprite_size[s], lookat);
//	}
//}

int32_t Pi3C::load_model(const std::string &path, const std::string &modelfile, const vec3f &pos, const bool addColliderGrid)
{
	return scene.loadModelOBJ(path, modelfile, pos, true, 1, addColliderGrid, nullptr);  // loadbarCallback);
}

int32_t Pi3C::load_model_and_collider(const std::string& path, const std::string& modelfile, const std::string& colliderfile, const vec3f& pos)
{
	Pi3Cmodel model; model.loadModelAndCollider(&resource, path, modelfile, 1, colliderfile, nullptr);  // loadbarCallback);
	uint32_t modelRef = scene.append3D(model);
	if (!pos.isZero()) scene.models[modelRef].move(pos);
	return modelRef;
}

bool Pi3C::is_running(bool doEvents)
{
	if (!has_started) {
		//resource.uploadMeshesToGPU();
		start_time = SDL_GetTicks();
		has_started = true;
	}
	resource.calls = 0;
	if (doEvents) do_events();
	return !window.hasquit();
}

void Pi3C::quit()
{
	window.setquit(true);
}

float Pi3C::getFloorHeight(const uint32_t modelRef, const vec3f pos)
{
	float ht = 1e8f;  //start from way up high!
	ht = scene.models[modelRef].collideFloor(&resource, nullptr, pos, ht, true);
	return ht;
}

void Pi3C::showFPS()
{
	gui.Begin();
	const std::string fpss = "FPS: " + std::to_string((int)fps);
	gui.Text(fpss, 0xffffffff);
}

void Pi3C::resize_window()
{
	winw = window.getWidth(); 
	winh = window.getHeight();
	scene.resize(Pi3Crecti(0, 0, winw, winh));
	for (uint32_t sc = 0; sc< backgrounds.size(); sc++) {
		model2D(backgrounds[sc])->updateRect2D(&resource, vec2f(0, 0), vec2f((float)width(), (float)height()));
	}
}

bool Pi3C::do_events()
{
	if (done_events) return false;
	done_events = true;

	const uint8_t * keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_ESCAPE]) window.setquit(true);

	//SDL_Delay(10);

	eventList = window.events();

	if (eventList.size()==0) return false;

	for (auto& ev : eventList) {
		switch (ev)
		{
		case SDL_WINDOWEVENT:
			if (window.resized) resize_window();
			break;
		case SDL_KEYDOWN:
			break;
		}
	}
	return true;

}

bool Pi3C::keyPress(char key) {
	const uint8_t *keys = SDL_GetKeyboardState(NULL);
	uint32_t scancode = SDL_GetScancodeFromKey(std::tolower(key));
	return (keys[scancode] != 0);
}

void Pi3C::swap_buffers() {
	frames++;

	// Calculate deltaTime
	float ticks = SDL_GetTicks() - last_time;
	last_time = SDL_GetTicks();
	deltaTime = ticks / 1000.0f;

	// Calculate fps
	fps = 1.0f / deltaTime;

	// Calculate average_fps
	average_fps = (float)frames / SDL_GetTicks() * 1000.0f;

	window.SwapBuffers();
	done_events = false;
}

void Pi3C::clear_window(uint32_t clearColour)
{
	if (clearColour != window.clearColour) window.setClearColour(clearColour);
	window.clear();
}

std::vector<std::string> Pi3C::get_dropfiles()
{
	std::vector<std::string> dropfiles;
	char* dropfile = window.ev.drop.file;
	dropfiles.push_back(dropfile);
	SDL_free(dropfile);
	return dropfiles;
}

//int32_t Pi3C::add_spriteArray(Pi3CspriteArray &spritearray, const std::string &file)
//{
//	//spritearray.meshRef = resource.addMesh(spritearray.samesh, true, true);
//	spritearray.meshRef = resource.addMesh(&spritearray.samesh);
//
//	Pi3Cmodel rectangles;
//	rectangles.meshRef = spritearray.meshRef;
//	rectangles.addTexture(&resource, file);
//	rectangles.material.illum = 1;
//	rectangles.material.alpha = 1.f;
//	return add_model_to_scene3D(rectangles);
//}

int32_t Pi3C::create_rect2D(const vec2f& pos, const vec2f& size, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel rect(&resource, Pi3Cshapes::rect(pos, size), MODEL_GPID, colour);
	return scene.append2D(rect, texfile);
}

int32_t Pi3C::create_cuboid(const vec3f& pos, const vec3f& size, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel cuboid(&resource, Pi3Cshapes::cuboid(pos, size), MODEL_GPID, colour);
	return scene.append3D(cuboid, texfile);
}

int32_t Pi3C::create_cylinder(const vec3f& pos, const float radius, const float height, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel cylinder(&resource, Pi3Cshapes::cylinder(pos, radius, height), MODEL_GPID, colour);
	return scene.append3D(cylinder, texfile);
}

int32_t Pi3C::create_cone(const vec3f& pos, const float radius, const float height, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel cone(&resource, Pi3Cshapes::cone(pos, radius, height), MODEL_GPID, colour);
	return scene.append3D(cone, texfile);
}

int32_t Pi3C::create_tube(const vec3f& pos, const float inner_radius, const float outer_radius, const float height, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel tube(&resource, Pi3Cshapes::tube(pos, inner_radius, outer_radius, height), MODEL_GPID, colour);
	return scene.append3D(tube, texfile);
}

int32_t Pi3C::create_tcone(const vec3f& pos, const float top_radius, const float bottom_radius, const float height, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel tcone(&resource, Pi3Cshapes::tcone(pos, bottom_radius, top_radius, height), MODEL_GPID, colour);
	return scene.append3D(tcone, texfile);
}

int32_t Pi3C::create_torus(const vec3f& pos, const float radius, const float ring_radius, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel torus(&resource, Pi3Cshapes::torus(pos, radius, ring_radius), MODEL_GPID, colour);
	return scene.append3D(torus, texfile);
}

int32_t Pi3C::create_sphere(const vec3f& pos, const float radius, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel sphere(&resource, Pi3Cshapes::sphere(pos, radius), MODEL_GPID, colour);
	return scene.append3D(sphere, texfile);
}

int32_t Pi3C::create_extrude(vec3f& pos, std::vector<std::vector<float>>& paths, float thickness, uint32_t colour, const std::string& texfile) {
	Pi3Cmodel extrude(&resource, Pi3Cshapes::extrude("", pos, paths, thickness), MODEL_GPID, colour);
	return scene.append3D(extrude, texfile);
}

int32_t Pi3C::create_extrude(vec3f& pos, std::vector<std::vector<vec2f>>& paths, float thickness, uint32_t colour, const std::string& texfile) {
	Pi3Cmodel extrude(&resource, Pi3Cshapes::extrude("", pos, paths, thickness), MODEL_GPID, colour);
	return scene.append3D(extrude, texfile);
}

int32_t Pi3C::create_lathe(const vec3f &pos, std::vector<vec2f> &path, const int edges, uint32_t colour, const std::string& texfile) {
	Pi3Cmodel lathe(&resource, Pi3Cshapes::lathe("", pos, path, edges), MODEL_GPID, colour);
	return scene.append3D(lathe, texfile);
}

int32_t Pi3C::create_spring(const vec3f& pos, const float radius, const float length, const int coils, const float coilradius, const int ringrots, const uint32_t colour, const std::string& texfile) {
	Pi3Cmodel spring(&resource, Pi3Cshapes::spring(pos, radius, length, coils, coilradius, ringrots), MODEL_GPID, colour);
	return scene.append3D(spring, texfile);
}

int32_t Pi3C::create_disk(const vec3f& pos, const float inner_radius, const float outer_radius, const uint32_t colour, const std::string& texfile) {
	Pi3Cmodel disk(&resource, Pi3Cshapes::disk(pos, inner_radius, outer_radius), MODEL_GPID, colour);
	return scene.append3D(disk, texfile);
}

int32_t Pi3C::create_plane(const vec3f& pos, const vec2f& size, const uint32_t xdivs, const uint32_t ydivs, int direction, const uint32_t colour, const std::string& texfile) {
	Pi3Cmodel plane(&resource, Pi3Cshapes::plane(pos, size, 1,1,0), MODEL_GPID, colour);
	return scene.append3D(plane, texfile);
}

int32_t Pi3C::create_elevationMap(const vec3f &pos, const vec3f &size, Pi3Ctexture &tex, const uint32_t xdivs, const uint32_t ydivs, const uint32_t colour, const std::string& texfile)
{
	Pi3Cmodel map(&resource, Pi3Cshapes::elevationMap(tex, pos, size, xdivs, ydivs, 0, vec2f(1.f,1.f),0), MODEL_GPID, colour);
	return scene.append3D(map, texfile);
}
