#include "Pi3C.h"
#include "Pi3CperlinNoise.h"
#include "Pi3CloadOptions.h"
//#include "Pi3Cshapes.h"
#include "PerlinGUI.h"

// ==========================================================================
// Pi3C Graphics Library Example - Perlin 2D example (by Tim Skillman)
// ==========================================================================
//
// The MIT License
//
// Copyright (c) 2019 Tim Skillman
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
// Dedicated to the One who has blessed me beyond my dreams ...
// Jesus Christ, King of Kings and Lord of Lords :-)
// =======================================================================


int main(int argc, char *argv[])
{
	loadOptions opts("options.txt");
	int screenWidth = opts.asInt("screenWidth");
	int screenHeight = opts.asInt("screenHeight");
	Pi3C pi3c(opts.asString("title"), screenWidth, screenHeight, opts.asBool("fullscreen"));

	uint32_t width = 256, height = 256;
	const std::shared_ptr<Pi3Ctexture> ptex(new Pi3Ctexture());

	float pz = 50.f;
	float pamp = 1.f;
	uint8_t octaves = 3;
	float freq = 5.f;

	//Pi3Ctexture ptex = Pi3Ctexture(width, height, 4);
	ptex->create(width, height, 4);
	Pi3CperlinNoise pn;
	uint32_t start_time = SDL_GetTicks();
	pn.createMap(ptex->GetTexels(), width, height, pz, octaves, pamp, freq);
	SDL_Log("Perlin time = %d", SDL_GetTicks() - start_time);
	//ptex->saveAsPNG("pmap.png");
	
	vec3f mapPos{ 0, -80.f, 0 };
	vec3f mapSize = vec3f((float)ptex->GetWidth(), 100.f, (float)ptex->GetHeight());

	int32_t rect = pi3c.create_rect2D(vec2f(30.f, 30.f), vec2f((float)width, (float)height), Pi3Ccolours::White);
	pi3c.model2D(rect)->addPicture(&pi3c.resource, *ptex);

	int32_t mapref = pi3c.create_elevationMap(mapPos, mapSize, *ptex, 128, 128, Pi3Ccolours::White,"");

	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));
	pi3c.scene.setFixedLight(0xffffff, vec3f(1500.f, 1500.f, 1500.f)); //transform sun position into scene
	pi3c.scene.setMatrix(vec3f(0, 0, 0), vec3f(0, 0, -300.f), vec3f(0.5f, 0, 0));

	perlinGUI perlingui(pi3c.gui);

	bool updateMap = false;

	while (pi3c.is_running())
	{
		pi3c.clear_window(Pi3Ccolours::DeepSkyBlue);

		pi3c.render3D();

		pi3c.render2D();

		perlingui.update(pi3c.gui, pamp, pz, freq, octaves, updateMap);

		if (pi3c.window.mouseUp() && updateMap) {
			pn.createMap(ptex->GetTexels(), width, height, pz, octaves, pamp, freq);
			ptex->update();
			pi3c.model3D(mapref)->updateMesh(&pi3c.resource, Pi3Cshapes::elevationMap(*ptex, mapPos, mapSize, 128, 128, 0, vec2f(1,1), 0));
			updateMap = false;
		}

		pi3c.model3D(mapref)->rotate(vec3f(0, 0.005f, 0));

		//pi3c.showFPS();
		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
