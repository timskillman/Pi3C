#include "Pi3C.h"

Pi3Cwindow Pi3C::window;
Pi3Cresource Pi3C::resource;

void Pi3C::init(const std::string &title, const uint32_t width, const uint32_t height, const bool fullscreen) {

	winopts.title = title;
	if (width > 0 && height > 0) {
		winopts.width = width;
		winopts.height = height;
		winopts.fullscreen = fullscreen;
	}
	window.initOptions(winopts);

	resource.init(); //resource.init must be called after window creation

	// Setup shader ..
	int32_t basicShaderRef = resource.addShader("Shaders/vs1.glsl", "Shaders/fs1.glsl");
	if (basicShaderRef < 0) {
		window.showErrorMessage("Shader Error!", resource.error());
		return;
	}

	scene.setResource(&resource);
	scene.selectShader(basicShaderRef);

	scene.setFog(0xffffff, 10000.f, 25000.f);
	scene.setViewport2D(Pi3Crecti(0, 0, window.getWidth(), window.getHeight()), 0.1f, 4000.f);
	scene.setPerspective3D(window.getWidth(), window.getHeight(), 800.f, 1.f, 5000.f);

	gui.init(&resource, &window);
	guifonts.push_back(gui.addFont("../../Resources/fonts/", "NotoSans-Regular.ttf", 18));

	bsMenu.font = guifonts[0];
	bsMenu.textColour = 0x202020;
	bsMenu.buttonColour = 0xffffff;
	bsMenu.minWidth = 60;
	bsMenu.minHeight = 24;
	bsMenu.top = 3;
	bsMenu.bottom = 3;
	bsMenu.vertGap = 0;
	bsMenu.left = 10;
	bsMenu.highlightColour = 0xc0c0c0;
}

Pi3Cmodel Pi3C::create_model_from_text(std::string &text, const uint32_t width, const uint32_t colour)
{
	Pi3Cmodel textModel;
	textModel.textModel(&resource, gui.getFont(guifonts[0]).get(), text, width);
	textModel.material.SetColDiffuse(colour);
	return textModel;
}

void Pi3C::update_sprite_position(const uint32_t spritesRef, const uint32_t spriteRef, const float x, const float y)
{
	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
	rm->updateSpriteCoords(resource.vertBuffer[rm->bufRef], spriteRef, x, y);
}

void Pi3C::update_sprite_rotated(const uint32_t spritesRef, const uint32_t spriteRef, const vec2f &pos, const vec2f &size, const float angle)
{
	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
	rm->updateSpriteCoordsRotated(resource.vertBuffer[rm->bufRef], spriteRef, pos, size, angle);
}

void Pi3C::update_sprite_transform(const uint32_t spritesRef, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const Pi3Cmatrix *scene_matrix, const vec2f &cent)
{
	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
	rm->updateSpriteTransformCoords(resource.vertBuffer[rm->bufRef], spriteRef, pos, size, scene_matrix, cent);
}

void Pi3C::update_sprite_billboard(const uint32_t spritesRef, const uint32_t spriteRef, const vec3f &pos, const vec2f &size, const vec3f &lookat)
{
	Pi3CspriteArray *rm = (Pi3CspriteArray*)(&resource.meshes[spritesRef]);
	rm->updateSpriteBillboard(resource.vertBuffer[rm->bufRef], spriteRef, pos, size, lookat);
}

int32_t Pi3C::load_model(const std::string &path, const std::string &file)
{
	return scene.loadModelOBJ(path, file, nullptr);  // loadbarCallback);
}

bool Pi3C::is_running()
{
	if (!has_started) {
		resource.uploadMeshesToGPU();
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

void Pi3C::resize_window()
{
	scene.resize(Pi3Crecti(0, 0, (float)window.getWidth(), (float)window.getHeight()));
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
