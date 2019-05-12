#pragma once

//#include <SDL.h>
#include "ui/SMUIform.h"
#include "SMscene.h"

class SMeditor {
public:

	SMeditor(SMUIform *form, SMscene *scene, std::string file, std::string uitexfile = "");
	~SMeditor();

	void editButtons(void *data);
	void toolButtons(void *data);
	void fileMenu(void *data);
	void editMenu(void *datay);
	void viewMenu(void *data);
	void canvas(void *data);
	void scrollCanvasV(void *data);
	void scrollCanvasH(void *data);

	void eventMouseDown(void *data);
	void loadingBar(void *data);

private:
	SMresource *resource;
	SMwindow *window;
	SMscene *scene;
	SMshader *shader;
	SMmaterial material;
};