#include "Pi3C.h"
#include "Pi3CperlinNoise.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cshapes.h"

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
	std::shared_ptr<Pi3Ctexture> ptex(new Pi3Ctexture());

	float pz = 50.f;
	float pamp = 1.f;
	uint8_t octaves = 3;
	float freq = 5.f;

	ptex->create(width, height, 4);
	Pi3CperlinNoise pn;
	uint32_t start_time = SDL_GetTicks();
	pn.createMap(ptex->GetTexels(), width, height, pz, octaves, pamp, freq);
	SDL_Log("Perlin time = %d", SDL_GetTicks() - start_time);
	//ptex->saveAsPNG("pmap.png");
	
	Pi3Cmodel rect;
	rect.createRect2D(&pi3c.resource, vec2f(30, 30), vec2f(width, height));
	rect.addPicture(&pi3c.resource, ptex);
	pi3c.add_model_to_scene2D(rect);

	Pi3Cmesh elevmap = Pi3Cshapes::elevationMap((*ptex), vec3f(0, -80, 0), vec2f(ptex->GetWidth(), ptex->GetHeight()), 100.f, 128, 128, 0, 0xffffffff);
	Pi3Cmodel map; map.appendMesh(&pi3c.resource, elevmap, false);
	map.material = (*pi3c.resource.defaultMaterial());  //The default material;
	map.material.SetColDiffuse(0xffffff);
	map.material.illum = 2;					//non shaded illumination
	uint32_t mapref = pi3c.add_model_to_scene3D(map);

	//pi3c.scene.setFog(opts.asHex("fogColour"), opts.asFloat("fogNear"), opts.asFloat("fogFar"));
	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));

	Pi3Cimgui& gui = pi3c.gui;
	gui.setImagePath("icons");
	Pi3Cimgui::rectStyle bsHeading;
	bsHeading.textColour = 0xffffff;
	bsHeading.highlightColour = 0x00ffff;
	bsHeading.buttonColour = 0x808080;
	bsHeading.top = 0;
	bsHeading.bottom = 0;
	bsHeading.left = 0;
	bsHeading.right = 0;

	float roty = 0;
	bool updateMap = false;
	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		pi3c.scene.setMatrix(vec3f(0, 0, 0), vec3f(0, 0, -300.f), vec3f(0.3f,roty,0));
		//roty += 0.001f;

		pi3c.scene.setFixedLight(0xffffff, vec3f(1500.f, -1500.f, -1500.f)); //transform sun position into scene
		pi3c.render3D();

		pi3c.render2D();

		double v = (double)pamp;
		gui.Begin();
		gui.setButtonStyle(bsHeading);

		gui.Label("Perlin Noise Map Generator");
		gui.nextLine();

		gui.Label("Smoothing:", 120);
		gui.sameLine();
		gui.SliderH("", 0, 2, v, 300, 24);
		if ((float)v != pamp) { pamp = (float)v; updateMap = true; }
		gui.nextLine();

		gui.Label("Octaves:", 120);
		gui.sameLine();
		v = (double)octaves;
		gui.SliderH("", 1, 10, v, 300, 24);
		if ((uint8_t)v != octaves) { octaves = (uint8_t)v; updateMap = true; }
		gui.nextLine();

		gui.Label("Z position:", 120);
		gui.sameLine();
		v = (double)pz;
		gui.SliderH("", 0, 100, v, 300, 24);
		if ((uint8_t)v != pz) { pz = (uint8_t)v; updateMap = true; }
		gui.nextLine();

		gui.Label("Scale:", 120);
		gui.sameLine();
		v = (double)freq;
		gui.SliderH("", 0, 100, v, 300, 24);
		if ((uint8_t)v != freq) { freq = (uint8_t)v; updateMap = true; }
		gui.nextLine();

		if (pi3c.window.mouseUp() && updateMap) {
			pn.createMap(ptex->GetTexels(), width, height, pz, octaves, pamp, freq);
			ptex->update();
			pi3c.scene.models[mapref].updateMesh(&pi3c.resource, Pi3Cshapes::elevationMap((*ptex), vec3f(0, -80, 0), vec2f(ptex->GetWidth(), ptex->GetHeight()), 100.f, 128, 128, 0, 0xffffffff));
			updateMap = false;
		}

		//pi3c.showFPS();
		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
