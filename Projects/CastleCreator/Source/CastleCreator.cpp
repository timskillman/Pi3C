#include "Pi3Cwin.h"
#include "Pi3CloadOptions.h"
#include "Editor.h"

// ==========================================================================
// Pi3C Graphics Library Example - Castle Creator (by Tim Skillman)
// ==========================================================================
//
// The MIT License
//
// Copyright (c) 2018 Tim Skillman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================


int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");

    // Create and initialize a window
	Pi3Cwindow::options winopts;
	winopts.title = opts.asString("title");
	winopts.fullscreen = opts.asBool("fullscreen");
	winopts.width = opts.asInt("screenWidth");
	winopts.height = opts.asInt("screenHeight");
	winopts.clearColour = 0x303030;
	static Pi3Cwindow window(winopts);

	Pi3CGL::showGLinfo();

	// Setup Editor and Resources ...
	static Pi3Cresource resource;
	resource.init();
	Editor castleEditor(&resource, &window);
	if (!castleEditor.initialised()) return 1;
	
	castleEditor.loadModels("CastleModels.txt", "CastleScene.txt");
		
	uint32_t frames = 0;
	uint32_t timer = SDL_GetTicks();

	while (!window.hasquit())
	{
		castleEditor.handleKeys();
		castleEditor.handleEvents();
		castleEditor.touchScene();
		castleEditor.tweens();
		castleEditor.animate();

		window.clear();
		castleEditor.render();

		frames++;
		window.SwapBuffers();	
	}

	uint32_t ticks = SDL_GetTicks() - timer;
	float avtime = (float)frames / ((float)ticks / 1000.f);
	SDL_Log("Average FPS(%d, %d) = %f ", ticks, frames, avtime);

	window.destroy();
	return 0;
}

