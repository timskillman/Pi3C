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

class rock {
public:

	rock(vec2f pos) { create(pos); }

	void create(const vec2f &pos) {
		this->pos = pos;
		dx = (float)(rand() % 1600) / 100.f - 8.f;
		size = (float)(rand() % 60) + 20.f;
		e = (float)(rand() % 100) / 100.f + 0.1f;
		gravity = -(float)(rand() % 20) - 10.f;
		dead = false;
		//col = (rand() % 255) | ((rand() % 255) << 8) | ((rand() % 255) << 16) | ((rand() % 255) << 24) | (255<<24);
	}

	void animate(const float d)
	{
		if (dead) return;

		pos += vec2f(dx, -gravity);
		if (pos.y < d) {
			if (std::fabs(gravity) < 1.0) {
				e = 0;
				pos.y = d;
				gravity = 0;
			}
			gravity = -gravity * 0.5f;
			pos.y -= gravity;
		}

		if (gravity == 0) {
			dx = dx * -0.95f;
			dead = (std::fabs(dx) < 0.1f);
		}
		
		gravity = gravity + e;
	}

	vec2f pos;
	float size = 1.f;
	float dx = 0;
	float e = 0.f;
	float gravity = 0.f;
	bool dead = false;
	uint32_t vertoffset = 0;
	uint32_t col = 0;

};

int main(int argc, char *argv[])
{
	Pi3C pi3c("Volcano");
	pi3c.window.setClearColour(0x0);

	std::vector<rock> boulders;
	Pi3Cmesh rectMesh;
	for (uint32_t r = 0; r < 10000; r++) {
		rock boulder(vec2f(pi3c.window.getWidth()/2, 250.f));
		boulder.vertoffset = rectMesh.vc;
		boulders.push_back(boulder);
		rectMesh.addRectangle(rectMesh.verts, vec3f(boulder.pos.x, boulder.pos.y, -5.f), vec3f(boulder.size, boulder.size, 0), vec2f((float)(rand() % 4)*0.25f , 0), vec2f(0.25f, 1.f));
	}
	rectMesh.vertSize = rectMesh.vc/ rectMesh.stride;
	rectMesh.bbox.bboxFromVerts(rectMesh.verts, 0, rectMesh.vc, rectMesh.stride);
	rectMesh.materialRef = 0;
	int rectsMesh = pi3c.resource.addMesh(rectMesh, true, true);
	Pi3Cmodel rectangles;
	rectangles.meshRef = rectsMesh;
	rectangles.addTexture(&pi3c.resource, "../../Resources/models/maps/lensflare2.png");
	rectangles.material.illum = 1;
	rectangles.material.alpha = 0.99f;
	int rectsRef = pi3c.add_model_to_scene2D(rectangles);

	//Pi3Cmodel textmodel = pi3c.create_model_from_text(livetext, mediaWidth - 40, 0xff202020);
	//int textRef = pi3c.add_model_to_scene2D(textmodel);

	Pi3Cimgui &gui = pi3c.gui;

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window();

		/*
		gui.Begin();

		//Menubar ...
		gui.setButtonStyle(pi3c.bsMenu);
		if (gui.BeginMenuBar()) {
			if (gui.BeginMenu("File")) {
				if (gui.MenuItem("New", "Ctrl+N")) {};
				if (gui.MenuItem("Open", "Ctrl+O")) {};
				if (gui.MenuItem("Save", "Ctrl+S")) {}
				if (gui.MenuItem("Quit", "")) pi3c.window.setquit(true);
			}
			if (gui.BeginMenu("Edit")) {
				if (gui.MenuItem("Copy", "Ctrl+C")) {}
				if (gui.MenuItem("Paste", "Ctrl+V")) {}
				if (gui.MenuItem("Delete", "Del")) {}
				if (gui.MenuItem("Undo", "Ctrl+X")) {}
				if (gui.MenuItem("Redo", "Ctrl+Y")) {}
			}
			if (gui.BeginMenu("Object")) {
				if (gui.MenuItem("Copy", "Ctrl+C")) {}
				if (gui.MenuItem("Paste", "Ctrl+V")) {}
				if (gui.MenuItem("Delete", "Del")) {}
				if (gui.MenuItem("Undo", "Ctrl+X")) {}
				if (gui.MenuItem("Redo", "Ctrl+Y")) {}
			}
			gui.EndMenuBar();
		}

		gui.setPosition((pi3c.window.getWidth()-mediaWidth)/2, 40);
		bool paperTouch = gui.renderRect(mediaWidth, mediaHeight);
		*/

		Pi3Cmesh &rm = pi3c.resource.meshes[rectsMesh];
		std::vector<float> &verts = pi3c.resource.vertBuffer[rm.bufRef];
		rm.vc = rm.vertOffset*rm.stride;
		for (uint32_t r = 0; r < boulders.size(); r++) {
			rock &boulder = boulders[r];
			boulder.animate(0);
			if (boulder.dead)
				boulder.create(vec2f((float)(pi3c.window.getWidth() / 2), 100)); //(float)(rand() % 500)
			rm.updateRectCoords(verts, vec3f(boulder.pos.x, boulder.pos.y, -5.f), vec3f(boulder.size, boulder.size, 0));
		}
		pi3c.resource.updateMesh(rectsMesh); //upload all the altered vertices for this mesh

		pi3c.render2D();

		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}