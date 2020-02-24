#include "Pi3C.h"
#include "Pi3CloadOptions.h"
#include "Pi3Cshapes.h"
#include "Pi3CmemContour.h"
#include "Pi3Cbbox2d.h"

// ==========================================================================
// Pi3C Graphics Library Example - Extruder 2D example (by Tim Skillman)
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
	Pi3C pi3c("Extruded Shape", 800, 600);
	
	Pi3CmlinPath lpath("raspberry");
	std::vector<std::vector<float>> floatPath = lpath.asFloats();
	vec3f pathCenter = vec3f(-lpath.bbox.centre().x, 0, -lpath.bbox.centre().y);
	float thickness = 10.f;

	pi3c.create_extrude(pathCenter, floatPath, thickness, Pi3Ccolours::White, "butterflies.png");
	//pi3c.create_cuboid(vec3f(0, 80, 0), vec3f(50, 50, 50), Pi3Ccolours::Yellow);

	pi3c.scene.setFixedLight(Pi3Ccolours::White, vec3f(150.f, 150.f, 150.f)); //transform sun position into scene

	float roty = 0;

	while (pi3c.is_running())
	{
		pi3c.clear_window();

		//const uint8_t *keystate = pi3c.window.getKeys();
		//if (keystate[SDL_SCANCODE_S]) {
		//	SDL_RWops *fo = SDL_RWFromFile("thumb.png", "wb");
		//	pi3c.scene.renderOffscreen(fo, 512, 512);
		//	SDL_RWclose(fo);
		//	glViewport(0, 0, screenWidth, screenHeight);
		//	pi3c.scene.setPerspective3D(screenWidth, screenHeight, opts.asFloat("perspective"), opts.asFloat("nearz"), opts.asFloat("farz"));
		//} 
		pi3c.scene.setMatrix(vec3f(0, 0, 0), vec3f(0, 0, -300.f), vec3f(roty,roty,0));
		roty += 0.03f;

		pi3c.render3D();

		
		pi3c.render2D();
		pi3c.showFPS();

		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
