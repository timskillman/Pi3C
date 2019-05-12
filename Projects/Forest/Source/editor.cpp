#include "editor.h"
#include <string>
#include <functional>

using namespace std::placeholders;

void SMeditor::fileMenu(void * data)
{

}

using namespace SMUIcontrols;

SMeditor::SMeditor(SMUIform *form, SMscene *scene, std::string file, std::string uitexfile)
{
	resource = form->resource;
	window = form->window;
	this->scene = scene;
	shader = form->shader;

	//Declare functions to be used for UI ...
	form->addFunction("filemenu", std::bind(&SMeditor::fileMenu, this, _1));
	form->addFunction("fileLoadingCB", std::bind(&SMeditor::loadingBar, this, _1));
	form->addFunction("editmenu", std::bind(&SMeditor::editMenu, this, _1));
	form->addFunction("viewmenu", std::bind(&SMeditor::viewMenu, this, _1));
	form->addFunction("toolButtons", std::bind(&SMeditor::toolButtons, this, _1));

	//Load UI image used for all Widget elements
	if (uitexfile != "") material.texRef = resource->loadTexture("", uitexfile, material.texID);

	//must declare all functions before form is loaded and created ...
	if (file != "") form->load(file.c_str());

	// Setup editor UI and bind functions to UI controls 
	// (control names must be the same as function names declared in addFunction) ...

	//create loading bar ...
	form->addControl("loadingBar", new loadingbar(resource, vec2f(window->getWidth() / 2.f - 100.f, window->getHeight() / 2.f - 20.f), vec2f(200, 40), 0));
	form->addEvent("loadingBar", "EvDraw", "fileLoadingCB");

	//Create window menus ...
	//form->addControl("filemenu", new SMUIcontrols::menu("File:New[CTRL+N],Open[CTRL+O],Save[CTRL+S],Save as|Exit[ALT+F4]"));
	//form->addControl("editmenu", new SMUIcontrols::menu("Edit:Cut[CTRL+X],Copy[CTRL+C],Paste[CTRL+V],Delete[DEL]"));
	//form->addControl("viewmenu", new SMUIcontrols::menu("View:Front,Back,Left,Right,Top,Bottom,Ortho,Perspective,Wireframe,Solid,Textured"));

	////Create buttons ...
	//form->addControl("toolButtons", new SMUIcontrols::button("Cube(cube.png)[F1],Cone(cone.png)[F2],Cylinder(cylinder.png),Sphere(sphere.png),Tcone(tcone.png),Torus(torus.png),Plane(plane.png)"));



	//Once all editor elements have been created, upload them to the GPU ...
	resource->uploadMeshesToGPU();
}

SMeditor::~SMeditor()
{

}

void SMeditor::editButtons(void *data)
{

}

void SMeditor::toolButtons(void *data)
{

}

void SMeditor::editMenu(void *data)
{
	std::string menukey = reinterpret_cast<char*>(data);

	if (menukey == "Cut") {

	}
	else if (menukey == "Copy") {

	}
	else if (menukey == "Paste") {

	}
	else if (menukey == "Delete") {

	}
}

void SMeditor::viewMenu(void *data)
{
	std::string menukey = reinterpret_cast<char*>(data);

	if (menukey == "Front") {

	}
	else if (menukey == "Back") {

	}
	else if (menukey == "Left") {

	}
	else if (menukey == "Right") {

	}
	else if (menukey == "Top") {

	}
	else if (menukey == "Bottom") {

	}
	else if (menukey == "Ortho") {

	}
	else if (menukey == "Perpective") {

	}
	else if (menukey == "Wireframe") {

	}
	else if (menukey == "Solid") {

	}
	else if (menukey == "Textured") {

	}
}

void SMeditor::canvas(void *data)
{

}

void SMeditor::scrollCanvasV(void *data)
{

}

void SMeditor::scrollCanvasH(void *data)
{

}

void SMeditor::eventMouseDown(void *data)
{
	SDL_Event *ev = reinterpret_cast<SDL_Event*>(data);

	switch (ev->type) {
	case SDL_MOUSEMOTION:
		//mousex = ev.motion.x;
		//mousey = ev.motion.y;
		//mouseXY = vec2f((float)ev.motion.x, (float)ev.motion.y);
		//mouseDeltaXY = mouseXY - oldMouseXY;
		//oldMouseXY = mouseXY;
		//wasDragging = false;
		//mouseup = false;
		//if (mousedown) mousedrag = true;
		break;
	}
}

void SMeditor::loadingBar(void *data)
{
	SMUIcontrols::loadingbar* loadbar = reinterpret_cast<SMUIcontrols::loadingbar*>(data);

	

	while (window->event()) {} //catch events

	window->clear();
	
	glDisable(GL_DEPTH_TEST);
	SMmatrix orthoMtx;
	orthoMtx.SetOrtho(0, (float)window->getWidth(), (float)window->getHeight(), 0, 0.1f, 20.f);
	shader->setProjectionMatrix(orthoMtx);
	loadbar->draw(resource, shader);
	glEnable(GL_DEPTH_TEST);

	window->SwapBuffers();
}
