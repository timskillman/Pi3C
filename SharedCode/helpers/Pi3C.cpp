#include "Pi3C.h"

Pi3Cwindow Pi3C::window;
Pi3Cresource Pi3C::resource;

void Pi3C::init(const std::string &title) {
	winopts.title = title;
	window.initOptions(winopts);
	//window.init(title.c_str(), winopts.width, winopts.height, winopts.sdlflags, winopts.antialiasLevel);

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
	scene.setViewport2D(Pi3Crecti(0, 0, (float)window.getWidth(), (float)window.getHeight()), 0.1f, 2000.f);

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

bool Pi3C::is_running()
{
	if (!has_started) {
		resource.uploadMeshesToGPU();
		start_time = SDL_GetTicks();
		has_started = true;
	}

	return !window.hasquit();
}

float Pi3C::getAverageFPS()
{
	uint32_t ticks = SDL_GetTicks() - start_time;
	return (float)frames / ((float)ticks / 1000.f);
}

void Pi3C::do_events()
{
	while (window.event()) {
		switch (window.ev.type)
		{
		case SDL_WINDOWEVENT:
			switch (window.ev.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				scene.setViewport2D(Pi3Crecti(0, 0, (float)window.getWidth(), (float)window.getHeight()), 0.1f, 2000.f);
				break;
			}
			break;
		}
	}
}