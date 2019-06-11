#include "Pi3C.h"
#include "Pi3Cstring.h"
#include "Pi3Cparticles.h"

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


int main(int argc, char *argv[])
{
	Pi3C pi3c("Snow", 1920, 1080, false);

	uint32_t bkg = pi3c.create_background("../../Resources/models/maps/snowpath.jpg");  // wintersnow (must be rendered before snowflakes)

	Pi3Cparticles snowParticles;
	Pi3Cmodel snowmodel = snowParticles.create(&pi3c.resource, vec3f(0, pi3c.height(), -10.f), Pi3Cparticles::ixyz(pi3c.width(), pi3c.height(), 0), 2, 8, 4, 5000);
	int snowRef = pi3c.add_model_to_scene2D(snowmodel);

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		snowParticles.animate(0);
		snowmodel.updateSpriteCoordsRotated(&pi3c.resource, snowParticles.position, snowParticles.sizes, snowParticles.angle);

		pi3c.render2D();

		pi3c.showFPS();
		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
