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


int main(int argc, char *argv[])
{
	Pi3C pi3c("Markdown Document Viewer");

	std::string livetext = loadString("passage.txt");

	float mediaWidth = 700;
	float mediaHeight = 900;

	Pi3Cmodel textmodel = pi3c.create_model_from_text(livetext, mediaWidth - 40, 0xff202020);
	int textRef = pi3c.add_model_to_scene2D(textmodel);

	Pi3Cimgui &gui = pi3c.gui;

	while (pi3c.is_running())
	{
		pi3c.do_events();
		pi3c.clear_window(0xF0F0F0);

		gui.Begin();

		//Menubar ...
		gui.setButtonStyle(pi3c.bsMenu);
		if (gui.BeginMenuBar("MenuBar")) {
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
			gui.EndMenuBar("MenuBar");
		}

		gui.setPosition((pi3c.window.getWidth()-mediaWidth)/2, 40);
		bool paperTouch = gui.renderRect(mediaWidth, mediaHeight);

		pi3c.scene.models2D[textRef].matrix.move(vec3f((pi3c.window.getWidth() - mediaWidth + 40) / 2, pi3c.window.getHeight()-40, -1.5));
		//pi3c.scene.models2D[textRef].textModel(&pi3c.resource, gui.getFont(pi3c.guifonts[0]).get(), livetext, 700); // +sin(rq)*800.f);

		pi3c.render2D();

		pi3c.swap_buffers();
	}

	SDL_Log("Average FPS = %f ", pi3c.getAverageFPS());

	pi3c.window.destroy();
	return 0;
}