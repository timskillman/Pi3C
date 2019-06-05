#include "Pi3C.h"
#include "Pi3Cstring.h"

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

#define fp 10
#define fpd (1<<fp)


class snowArray {
public:

	snowArray(uint32_t w, uint32_t h) : width(w), height(h) {}

	float speed = 0.0001f;
	uint32_t width = 0;
	uint32_t height = 0;

	std::vector<vec3f> position;
	std::vector<vec3f> dir;
	std::vector<vec2f> sizes;
	std::vector<uint32_t> flakeType;
	std::vector<float> angle;
	std::vector<float> angleinc;

	void update(const size_t ref, const vec3f &pos, const vec2f size, const uint32_t flakeRef)
	{
		position[ref] = pos;
		dir[ref] = vec3f(0, (float)-((rand() % 100000) + 10000), 0);
		sizes[ref] = size;
		flakeType[ref] = flakeRef;
		angle[ref] = (float)(rand() % 628) / 100.f;
		angleinc[ref] = (float)(rand() % 1000 - 500) *speed;
	}

	void create(const vec3f &pos, const vec2f size, const uint32_t flakeRef) {
		position.push_back(pos);
		dir.push_back(vec3f(0, (float)-((rand()%100000)+10000), 0));
		sizes.push_back(size);
		flakeType.push_back(flakeRef);

		angle.push_back((float)(rand() % 628) / 100.f);
		angleinc.push_back((float)(rand() % 1000 - 500) *speed);
	}

	void animate(const float d)
	{
		for (uint32_t i = 0; i < position.size(); i++) {
			position[i] += dir[i] * speed;
			dir[i].x += (rand() % 1000) - 500;
			angle[i] += angleinc[i];
			if (position[i].y < d) update(i, vec3f((float)(rand() % width), (float)(height + rand() % height + 100), -10.f), sizes[i], rand() % 4); // dead[i] = true;
		}
	}
};

#define random_top vec3f((float)(rand() % pi3c.width()), (float)(pi3c.height() + rand() % pi3c.height() + 100), -10.f)

int main(int argc, char *argv[])
{
	Pi3C pi3c("Snow", 1920, 1080, false);
	pi3c.window.setClearColour(0x0);
	pi3c.scene.setFog(0x000000, 5000, 6000);

	uint32_t bkg = pi3c.create_background("../../Resources/models/maps/snowpath.jpg");  // wintersnow (must be rendered before snowflakes)

	uint32_t r = 0;
	uint32_t snowNo = 5000;
	snowArray snow(pi3c.width(), pi3c.height());
	Pi3Cmesh snowMesh;
	while (r < snowNo) {
		vec3f pos = random_top;
		float flakesize = rand() % 8 + 2;
		vec2f fsize = vec2f(flakesize, flakesize);
		uint32_t type(rand() % 4);
		snow.create(pos, fsize, type);
		snowMesh.addRect(pos, fsize, vec2f((float)(type)*0.25f, 0.f), vec2f(0.25f, 0.99f));
		r++;
	}

	Pi3Cmodel snowflakeModel;
	snowflakeModel.meshRef = pi3c.resource.insertMesh(&snowMesh);
	snowflakeModel.addTexture(&pi3c.resource, "../../Resources/models/maps/snowflakes4b.png");
	snowflakeModel.material.illum = 1;
	snowflakeModel.material.alpha = .99f;
	int snowRef = pi3c.add_model_to_scene2D(snowflakeModel);

	Pi3Cimgui &gui = pi3c.gui;

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		snow.animate(0);
		snowflakeModel.updateSpriteCoordsRotated(&pi3c.resource, snow.position, snow.sizes, snow.angle);

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
