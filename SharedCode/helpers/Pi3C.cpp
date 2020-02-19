#include "Pi3C.h"

Pi3Cwindow Pi3C::window;
Pi3Cresource Pi3C::resource;

Pi3C::Pi3C(const Pi3Cwindow::options& winopts)
{
	init(winopts);
}

Pi3C::Pi3C(const std::string &title, const uint32_t width, const uint32_t height, const bool fullscreen) 
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
	int32_t basicShaderRef = resource.addShader("Shaders/vs1.glsl", "Shaders/fs1.glsl");
	if (basicShaderRef < 0) {
		window.showErrorMessage("Shader Error!", resource.error());
		return;
	}

	
	scene.init(&resource);
	scene.selectShader(basicShaderRef);

	scene.setFog(0xffffff, 10000.f, 25000.f);
	scene.setViewport2D(Pi3Crecti(0, 0, winw, winh), 0.1f, 4000.f);
	scene.setPerspective3D(winw, winh, PSPVALUE, 0.1f, 5000.f);

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
	textModel.textModel(&resource, gui.getFont(guifonts[0]).get(), text, width, Pi3Cfont::RD_HTML);
	textModel.material.SetColDiffuse(colour);
	return textModel;
}

uint32_t Pi3C::create_background(const std::string &path, const std::string &file)
{
	Pi3Cmodel background;

	Pi3Cmesh rect;
	rect.addRect(vec3f(0, winh, -20.f), vec2f(winw, winh));
	//background.meshRef = resource.addMesh(rect);
	background.meshRef = resource.addMesh(&rect);

	background.addTexture(&resource, (file!="" && path!="") ? path+"/"+file : path);
	background.material.illum = 1;
	background.material.alpha = 1.f;
	return add_model_to_scene2D(background);
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

int32_t Pi3C::load_model(const std::string &path, const std::string &file, const vec3f &pos)
{
	int32_t modelRef = scene.loadModelOBJ(path, file, true, nullptr);  // loadbarCallback);
	if (!pos.isZero()) scene.models[modelRef].move(pos);
	return modelRef;
}

int32_t Pi3C::load_model_and_collider(const std::string& path, const std::string& modelfile, const std::string& colliderfile, const vec3f& pos)
{
	Pi3Cmodel model; model.loadModelAndCollider(&resource, path, modelfile, colliderfile, nullptr);  // loadbarCallback);
	uint32_t modelRef = scene.append3D(model);
	if (!pos.isZero()) scene.models[modelRef].move(pos);
	return modelRef;
}

bool Pi3C::is_running()
{
	if (!has_started) {
		//resource.uploadMeshesToGPU();
		start_time = SDL_GetTicks();
		has_started = true;
	}
	resource.calls = 0;
	return !window.hasquit();
}

float Pi3C::getAverageFPS()
{
	uint32_t ticks = SDL_GetTicks() - start_time;
	return (float)frames / ((float)ticks / 1000.f);
}

float Pi3C::getCurrentFPS()
{
	uint32_t ticks = SDL_GetTicks() - last_time;
	if (ticks>1000) {
		float currentFPS = (float)fps / ((float)ticks / 1000.f);
		fps = 0; 
		last_time = SDL_GetTicks();
		lastFPS = currentFPS;
	}
	return lastFPS;
}

void Pi3C::showFPS()
{
	gui.Begin();
	const std::string fps = "FPS:" + std::to_string((int)getCurrentFPS());
	gui.Text(fps);
}

void Pi3C::resize_window()
{
	winw = window.getWidth(); winh = window.getHeight();
	scene.resize(Pi3Crecti(0, 0, (float)winw, (float)winh));
}

bool Pi3C::do_events()
{
	const uint8_t * keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_ESCAPE]) window.setquit(true);

	//SDL_Delay(10);

	if (!window.event()) return false;

	switch (window.ev.type)
	{
	case SDL_WINDOWEVENT:
		switch (window.ev.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			resize_window();
			break;
		}
		break;
	case SDL_KEYDOWN:
		break;
	}
	return true;

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
