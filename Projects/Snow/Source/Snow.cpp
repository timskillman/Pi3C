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

class snow {
public:

	snow(const vec3f &pos, const uint32_t size, const uint32_t flakeRef) { create(pos, size, flakeRef); }

	vec3f pos;
	vec3f dir;
	float speed = 1.f;

	float size = 1.f;
	float angle = 0;
	float angleinc = 0;
	uint32_t life = 0;
	bool dead = false;
	uint32_t flakeRef = 0;

	void create(const vec3f &pos, const uint32_t size, const uint32_t flakeRef) {
		speed = 0.0001f;
		this->pos = pos;
		dir = vec3f(0, (float)-((rand()%100000)+10000), 0);
		this->size = (float)size;// (float)(rand() % 20) + 3.f;
		this->flakeRef = flakeRef;

		angle = (float)(rand() % 628) / 100.f;
		angleinc = (float)(rand() % 1000 - 500) *speed;

		life = 1000 + rand() % 10000;
		dead = false;
	}

	void animate(const float d)
	{
		pos += dir*speed;
		dir.x += (rand() % 1000) - 500;
		angle += angleinc;
		if (pos.y < d) dead = true;
	}
};

#define random_top vec3f((float)(rand() % pi3c.width()), (float)(pi3c.height() + rand() % pi3c.height() + 100), -10.f)

int main(int argc, char *argv[])
{
	Pi3C pi3c("Snow", 1920, 1080, true);
	pi3c.window.setClearColour(0x0);

	uint32_t bkg = pi3c.create_background("../../Resources/models/maps/snowpath.jpg");  // wintersnow (must be rendered before snowflakes)

	std::vector<snow> snowParticles;
	Pi3CspriteArray sprites;
	for (uint32_t r = 0; r < 2000; r++) {
		snow flake(random_top, rand() % 18 + 2, rand() % 4);
		snowParticles.push_back(flake);
		sprites.addSprite(flake.pos, vec2f(flake.size, flake.size), vec2f((float)(flake.flakeRef)*0.25f, 0), vec2f(0.25f, 1.f));
	}
	int spritesRef = pi3c.resource.addMesh(sprites, true, true);

	Pi3Cmodel snowflakeModel;
	snowflakeModel.meshRef = spritesRef;
	snowflakeModel.addTexture(&pi3c.resource, "../../Resources/models/maps/snowflakes4b.png");
	snowflakeModel.material.illum = 1;
	snowflakeModel.material.alpha = .99f;
	int snowRef = pi3c.add_model_to_scene2D(snowflakeModel);

	Pi3Cimgui &gui = pi3c.gui;

	float fps = 20;

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		for (uint32_t r = 0; r < snowParticles.size(); r++) {
			snow &flake = snowParticles[r];
			flake.animate(0);
			if (flake.dead) flake.create(random_top, rand() % 18 + 2, rand() % 4);
			pi3c.update_sprite_rotated(spritesRef, r, vec2f(flake.pos.x, flake.pos.y), vec2f(flake.size, flake.size), flake.angle);
		}
		pi3c.resource.updateMesh(spritesRef); //upload all the altered vertices for this mesh

		pi3c.render2D();

		gui.Begin();
		std::string fps = "FPS:" + std::to_string((int)pi3c.getCurrentFPS());
		gui.Text(fps);

		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}
