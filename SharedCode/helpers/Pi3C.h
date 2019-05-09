#pragma once

#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cscene.h"
#include "Pi3Cimgui.h"
#include "Pi3Cmodel.h"
#include <vector>

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
// Dedicated to the One who changed my life in the most amazing ways ...
// Jesus Christ, King of Kings and Lord of Lords :-)
// =======================================================================


class Pi3C {
public:
	Pi3C() {}
	Pi3C(const char * title) { init(title); }

	void init(const std::string &title);

	Pi3Cmodel create_model_from_mesh(const Pi3Cmesh &mesh, const uint32_t colour = 0xffffff) { return Pi3Cmodel(&resource, mesh, colour); }
	Pi3Cmodel create_model_from_text(std::string &text, const uint32_t width, const uint32_t colour = 0x303030);
	uint32_t add_model_to_scene2D(const Pi3Cmodel &model) { return scene.append2D(model); }
	uint32_t add_model_to_scene3D(const Pi3Cmodel &model) { return scene.append3D(model); }

	bool is_running();
	void do_events();
	void clear_window() { window.clear(); }
	void swap_buffers() { frames++; window.SwapBuffers(); }
	float getAverageFPS();

	Pi3Cwindow::options winopts;
	static Pi3Cwindow window;
	static Pi3Cresource resource;
	Pi3Cscene scene;

	Pi3Cimgui gui;
	std::vector<std::string> guifonts;

	Pi3Cimgui::rectStyle bsMenu;

private:
	bool has_started = false;
	uint32_t frames = 0;
	uint32_t start_time = 0;
};