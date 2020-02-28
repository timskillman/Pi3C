#include "Pi3C.h"
#include "Pi3Cstring.h"
#include "Pi3Cparticles.h"
#include "Pi3Csprite.h"

// ==========================================================================
// Pi3C Graphics Library Example - Snow 2D example (by Tim Skillman)
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


int main(int argc, char* argv[])
{
	uint32_t scw = 800;
	uint32_t sch = 600;
	Pi3C pi3c("Snow", scw, sch, false);
	
	uint32_t bkg = pi3c.add_background("../../Resources/models/maps/snowpath.jpg");
	pi3c.showBackground(0);

	Pi3Cparticles snowParticles;
	vec3f startpos = vec3f(0.f, (float)pi3c.height(), -10.f);
	Pi3Cparticles::ixyz startsize = Pi3Cparticles::ixyz(scw, sch, 0);

	Pi3CspriteSheetInfo snowsprite;
	snowsprite.filename = "../../Resources/models/maps/snowflakes4b.png";
	snowsprite.tileX = 4;
	snowsprite.tileY = 1;
	snowsprite.frameCount = 4;

	snowParticles.create(&pi3c.resource, 4000, startpos, startsize, 2, 8, snowsprite);

	snowParticles.speed = 0.0001f;

	int snowRef = pi3c.add_model_to_scene2D(snowParticles.model);

	while (pi3c.is_running())
	{
		if (pi3c.window.resized) {
			snowParticles.updateStartArea(Pi3Cparticles::ixyz(pi3c.width(), pi3c.height(),0));
		}

		pi3c.clear_window();

		snowParticles.animate(0);
		snowParticles.updateParticleCoordsRotated(snowParticles.position, snowParticles.sizes, snowParticles.angle);

		pi3c.render2D();

		pi3c.showFPS();
		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
