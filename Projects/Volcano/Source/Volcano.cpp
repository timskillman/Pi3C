#include "Pi3C.h"
#include "Pi3Cstring.h"

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
// Dedicated to the One who changes my life in the most amazing ways ...
// Jesus Christ, King of Kings and Lord of Lords :-)
// =======================================================================

#define fp 10
#define fpd (1<<fp)

class rock {
public:

	rock(const int32_t x, const int32_t y) { create(x, y); }

	int32_t px = 0;
	int32_t py = 0;
	float size = 1.f;
	int32_t dx = 0;
	int32_t e = 0;
	int32_t gravity = 0;
	float angle = 0;
	float angleinc = 0;
	bool dead = false;
	//uint32_t col = 0;

	void create(const int32_t x, const int32_t y) {
		px = x << fp;
		py = y << fp;
		dx = rand() % (fpd<<4) - (fpd<<3);
		size = (float)(rand() % 60) + 7.f;
		angle = (float)(rand() % 628) / 100.f;
		angleinc = (float)(rand() % 100) / 1000.f -0.05f;
		e = rand() % fpd + (fpd>>3);
		gravity = -(rand() % (20<<fp)) - (10<<fp);
		dead = false;
		//col = (rand() % 255) | ((rand() % 255) << 8) | ((rand() % 255) << 16) | ((rand() % 255) << 24) | (255<<24);
	}

	void animate(const float d)
	{
		if (dead) return;

		px += dx;
		py -= gravity;

		if (py < d) {
			if (gravity>-fpd && gravity < fpd) {
				e = 0;
				py = d;
				gravity = 0;
			}
			gravity = -(gravity>>1);
			py -= gravity;
		}

		if (gravity == 0) {
			dx -= (dx>>7);
			dead = (dx > -1024 && dx < 1024);
		}
		
		angle += angleinc;
		gravity += e;
	}

};

int main(int argc, char *argv[])
{
	Pi3C pi3c("Volcano", 1920, 1080, true);
	pi3c.window.setClearColour(0x0);

	std::vector<rock> boulders;
	Pi3CspriteArray sprites;
	for (uint32_t r = 0; r < 1000; r++) {
		rock boulder(pi3c.window.getWidth()/2, 250);
		boulders.push_back(boulder);
		sprites.addSprite(
			vec3f((float)(boulder.px>>fp), (float)(boulder.py>>fp), -10.f), 
			vec2f(boulder.size, boulder.size), 
			vec2f((float)(rand() % 4)*0.25f , 0), vec2f(0.25f, 1.f));
	}
	int spritesRef = pi3c.resource.addMesh(sprites, true, true);

	Pi3Cmodel rectangles;
	rectangles.meshRef = spritesRef;
	rectangles.addTexture(&pi3c.resource, "../../Resources/models/maps/lavarocks.png"); // snowflakes4 //lavarocks
	rectangles.material.illum = 1;
	rectangles.material.alpha = 0.99f;
	int rectsRef = pi3c.add_model_to_scene2D(rectangles);

	uint32_t bkg = pi3c.create_background("../../Resources/models/maps/Volcano.png");

	uint32_t degsize = 360;
	std::vector<int32_t> costable;
	costable.resize(degsize);
	float degrad = PI2 / (float)degsize;
	for (size_t a = 0; a < degsize; a++) {
		costable[a] = (uint32_t)(cos(degrad*(float)a) * 1000.f);
	}

	Pi3Cimgui &gui = pi3c.gui;

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		for (uint32_t r = 0; r < boulders.size(); r++) {
			rock &boulder = boulders[r];
			boulder.animate(0);
			if (boulder.dead) boulder.create(pi3c.window.getWidth() / 2, pi3c.window.getHeight() *0.41f);
			pi3c.update_sprite_rotated(spritesRef, r, vec2f((float)(boulder.px >> fp), (float)(boulder.py >> fp)), vec2f(boulder.size, boulder.size), boulder.angle);
		}
		pi3c.resource.updateMesh(spritesRef); //upload all the altered vertices for this mesh

		pi3c.render2D();
		gui.Begin();
		gui.Text("FPS:" + std::to_string((int)pi3c.getCurrentFPS()));

		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
