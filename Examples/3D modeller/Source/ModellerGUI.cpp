#include "ModellerGUI.h"
#include "Modeller.h"
#include <sstream>
#include <iomanip>

void MGui::init(loadOptions &opts, Pi3Cresource * resource, Pi3Cwindow *window)
{
	gui.init(resource, window);

	// Get GUI fonts
	//resource->addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 80);
	largeFont = gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 28);
	mediumFont = gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 20);
	smallFont = gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 16);

	gui.setImagePath("assets/icons");

	uint32_t backColour = 0xcccccc;
	uint32_t white = 0xffffff;
	uint32_t highlight = 0xeeeeee;
	uint32_t selectColour = 0x00ffff;

	// Setup GUI button styles ...
	bsMenu.font = smallFont;
	bsMenu.textColour = 0x0;
	bsMenu.buttonAlpha = 1.f;
	bsMenu.buttonColour = white;
	bsMenu.minHeight = 32;
	bsMenu.minWidth = 70;
	bsMenu.vertGap = 0;
	bsMenu.left = 5;
	bsMenu.highlightColour = 0xc0c0c0;

	bsHeading.font = largeFont;
	bsHeading.textColour = white;
	bsHeading.highlightColour = 0x00ffff;
	bsHeading.buttonColour = backColour;
	bsHeading.top = 0;
	bsHeading.bottom = 0;
	bsHeading.left = 0;
	bsHeading.right = 0;

	bsIcons.font = largeFont;
	bsIcons.textColour = white;
	bsIcons.buttonColour = backColour;
	bsIcons.highlightColour = highlight;
	bsIcons.minWidth = 32;
	bsIcons.minHeight = 32;
	bsIcons.top = 0;
	bsIcons.bottom = 0;
	bsIcons.left = 0;
	bsIcons.right = 0;
	bsIcons.horizGap = 0;
	bsIcons.sameLine = true;
	bsIcons.selectColour = selectColour;

	bsButtons.font = largeFont;
	bsButtons.textColour = white;
	bsButtons.buttonColour = backColour;
	bsButtons.highlightColour = highlight;
	bsButtons.justify = Pi3Cimgui::CENTRE;
	bsButtons.align = Pi3Cimgui::CENTRE;
	bsButtons.minWidth = 40;
	bsButtons.minHeight = 40;
	bsButtons.top = 0;
	bsButtons.bottom = 0;
	bsButtons.left = 0;
	bsButtons.right = 0;
	bsButtons.vertGap = 0;
	bsButtons.selectColour = selectColour;
	//bsButtons.sameLine = true;

	bsItems.font = smallFont;
	bsItems.buttonAlpha = 0.8f;
	bsItems.buttonColour = white;
	bsItems.textColour = 0x404040;
	bsItems.minWidth = 140;
	bsItems.minHeight = 40;
	bsItems.justify = Pi3Cimgui::CENTRE;
	bsItems.align = Pi3Cimgui::BOTTOM;
	bsItems.selectColour = selectColour;

	bsDialog.font = smallFont;
	bsDialog.buttonAlpha = 0.8f;
	bsDialog.buttonColour = backColour;
	bsDialog.textColour = 0x404040;
	bsDialog.minWidth = 32;
	bsDialog.minHeight = 32;
	bsDialog.justify = Pi3Cimgui::CENTRE;
	bsDialog.align = Pi3Cimgui::CENTRE;
	bsDialog.selectColour = selectColour;
	bsDialog.highlightColour = Pi3Ccolours::Yellow;
}

void MGui::renderYellowBorder(uint32_t currentSelView)
{
	Pi3Crecti rec;
	switch (currentSelView) {
	case viewInfo::TOPLEFT: rec = getRectTopLeft(); break;
	case viewInfo::TOPRIGHT:rec = getRectTopRight(); break;
	case viewInfo::BOTTOMLEFT:rec = getRectBottomLeft(); break;
	case viewInfo::BOTTOMRIGHT:rec = getRectBottomRight(); break;
	case viewInfo::FULL:rec = getRectFull(); break;
	}

	if (rec.width > 0) {
		Pi3Cmodel rect;
		float rw = 3.f;
		uint32_t col = 0xff00ffff;
		rect.matrix.setz(-1.f);
		rect.createRect2D(gui.resource, vec2f((float)rec.x, (float)rec.y), vec2f((float)rec.width, rw), col);
		rect.renderBasic(gui.resource);
		rect.createRect2D(gui.resource, vec2f((float)rec.x, (float)(rec.y + rec.height) - rw), vec2f((float)rec.width, rw), col);
		rect.renderBasic(gui.resource);
		rect.createRect2D(gui.resource, vec2f((float)rec.x, (float)rec.y), vec2f(rw, (float)rec.height), col);
		rect.renderBasic(gui.resource);
		rect.createRect2D(gui.resource, vec2f((float)(rec.x + rec.width) - rw, (float)rec.y), vec2f(rw, (float)rec.height), col);
		rect.renderBasic(gui.resource);
	}
}

Pi3Crecti MGui::getRectBottomRight()
{
	return Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX) + dbh, botbarHeight, (int)((float)workWidth * ibx), (int)((float)workHeight * iby));
}

Pi3Crecti MGui::getRectBottomLeft()
{
	return Pi3Crecti(leftbarWidth, botbarHeight, (int)((float)workWidth * dragBarX) - dbh, (int)((float)workHeight * iby));
}

Pi3Crecti MGui::getRectTopRight()
{
	return Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX) + dbh, botbarHeight + (int)((float)workHeight * iby) + dbh, (int)((float)workWidth * ibx), (int)((float)workHeight * dragBarY) - dbh * 3);
}

Pi3Crecti MGui::getRectTopLeft()
{
	return Pi3Crecti(leftbarWidth, botbarHeight + (int)((float)workHeight * iby) + dbh, (int)((float)workWidth * dragBarX) - dbh, (int)((float)workHeight * dragBarY) - dbh * 3);
}

Pi3Crecti MGui::getRectFull()
{
	return Pi3Crecti(leftbarWidth, botbarHeight, workWidth, workHeight - dbh * 3);
}


bool MGui::draggingBars()
{
	return draggingBarX || draggingBarY;
}

float dragLimit(float& dragPos, float pos)
{
	dragPos = pos;
	if (dragPos < 0.02f) dragPos = 0.02f;
	if (dragPos > 0.98f) dragPos = 0.98f;
	return 1.f - dragPos;
}

void MGui::dragViewportBars(Modeller * md, Pi3Cpointi& wpos, int midht)
{
	bool mb = md->window->mouse.LeftButton;
	int mx = md->window->mouse.x;
	int my = md->window->mouse.y;

	if (md->fullview == viewInfo::INACTIVE) {
		//Drag bar horizontal ...
		gui.setPosition(leftbarWidth + (int)(dragBarX * (float)workWidth - dragBarThickness * 0.5f), wpos.y);

		bool touchDragBarH = gui.renderRect((int)dragBarThickness, midht);
		if (touchDragBarH || draggingBarX) md->setDragBarH(true);

		if (touchDragBarH && mb && !draggingBars()) {
			draggingBarX = true;
		}
		else if (draggingBarX && mb) {
			ibx = dragLimit(dragBarX, (float)(mx - leftbarWidth) / (float)workWidth);
		}
		else {
			draggingBarX = false;
		}

		//Drag bar vertical ...
		gui.setPosition(leftbarWidth, topbarHeight + menuHeight + (int)((float)workHeight*dragBarY - dragBarThickness * 0.5f));

		bool touchDragBarV = gui.renderRect(workWidth, (int)dragBarThickness);
		if (touchDragBarV || draggingBarY) md->setDragBarV(true);

		if (touchDragBarV && mb && !draggingBars()) {
			draggingBarY = true;
		}
		else if (draggingBarY && mb) {
			iby = dragLimit(dragBarY, (float)(my - topbarHeight - menuHeight) / (float)workHeight);
		}
		else {
			draggingBarY = false;
		}

		if (!(touchDragBarH || touchDragBarV || draggingBarX || draggingBarY)) {
			md->setDragBarH(false);
		}
	}
}

void MGui::drawBorders(Modeller * md, Pi3Cpointi& wpos)
{
	int winWidth = md->window->getWidth();
	int winHeight = md->window->getHeight();

	int midht = winHeight - wpos.y - botbarHeight;
	gui.renderRect(leftbarWidth, midht);
	gui.movePosition(winWidth - rightbarWidth, 0);
	gui.renderRect(rightbarWidth, midht);
	gui.setPosition(0, wpos.y + midht);
	gui.renderRect(winWidth, botbarHeight);

	dragViewportBars(md, wpos, midht);
}

void MGui::doMenus(Modeller * md)
{
	//Menubar ...
	gui.setButtonStyle(bsMenu);
	if (gui.BeginMenuBar("MenuBar")) {
		if (gui.BeginMenu("File")) {
			if (gui.MenuItem("New", "Ctrl+N")) md->clearScene();
			if (gui.MenuItem("Open", "Ctrl+O")) {} // open();
			if (gui.MenuItem("Save", "Ctrl+S")) saveAll(md);
			if (gui.MenuItem("Quit", "Esc")) md->window->setquit(true);
			gui.EndMenu();
		}
		if (gui.BeginMenu("Edit")) {
			if (gui.MenuItem("Select All", "Ctrl+A")) md->selectAll();
			if (gui.MenuItem("Clear selections", "")) md->clearSelections();
			if (gui.MenuItem("Copy", "Ctrl+C")) {}
			if (gui.MenuItem("Paste", "Ctrl+V")) {}
			if (gui.MenuItem("Delete", "Del")) {}
			if (gui.MenuItem("Undo", "Ctrl+X")) {}
			if (gui.MenuItem("Redo", "Ctrl+Y")) {}
			gui.EndMenu();
		}
		if (gui.BeginMenu("Object")) {
			if (gui.MenuItem("Create", "Ctrl+C")) {}
			if (gui.MenuItem("Array tool", "Ctrl+V")) {}
			gui.EndMenu();
		}
		gui.EndMenuBar("MenuBar");
	}
}


void MGui::doEditToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb)
{
	int icw = bsIcons.halfWidth(); // (int)((float)bsIcons.minWidth*0.5f);
	int ich = bsIcons.minHeight;
	int idw = bsIcons.thirdWidth(); // (int)((float)bsIcons.minWidth*0.3f);

	if (gui.BeginGroupHorizontal("rendl.png", icw, ich)) {

		if (gui.ButtonImage("butNew.png") && mb) md->clearScene();
		if (gui.ButtonImage("butOpen.png") && mb) {}
		if (gui.ButtonImage("butSave.png") && mb) saveAll(md);
		gui.renderBackIcon("butDiv.png", idw, ich);

		if (gui.ButtonImage("butArrow.png", md->editMode == Modeller::ED_SELECT) && mb) md->setEditMode(Modeller::ED_SELECT);
		if (gui.ButtonImage("butMove.png", md->editMode == Modeller::ED_MOVE) && mb) md->setEditMode(Modeller::ED_MOVE);
		if (gui.ButtonImage("butRotate.png", md->editMode == Modeller::ED_ROTATE) && mb) md->setEditMode(Modeller::ED_ROTATE);
		if (gui.ButtonImage("butScale.png", md->editMode == Modeller::ED_SCALE) && mb) md->setEditMode(Modeller::ED_SCALE);

		gui.renderBackIcon("butDiv.png", idw, ich);

		if (gui.ButtonImage("butSelectAll.png") && mb) md->selectAll();
		if (gui.ButtonImage("butCut.png") && mb) {}
		if (gui.ButtonImage("butPaste.png") && mb) {}
		if (gui.ButtonImage("butCopy.png") && mb) md->snapshot();

		gui.renderBackIcon("butDiv.png", idw, ich);

		if (gui.ButtonImage("butUndo.png") && mb) {}
		if (gui.ButtonImage("butRedo.png") && mb) {}

		gui.EndGroupHorizontal("rendr.png", icw, ich);
	}
}

void MGui::doTransformToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb)
{
	if (gui.BeginGroupHorizontal("rendl.png", iconstyle.halfWidth(), iconstyle.minHeight)) {
		if (gui.ButtonImage("butMirror.png") && mb) {}
		if (gui.ButtonImage("butFlip.png") && mb) {}
		gui.EndGroupHorizontal("rendr.png", iconstyle.halfWidth(), iconstyle.minHeight);
	}
}

void MGui::doSceneToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu)
{
	//gui.setButtonStyle(iconstyle);

	if (gui.BeginGroupHorizontal("rendl.png", iconstyle.halfWidth(), iconstyle.minHeight)) {
		if (gui.ButtonImage("butFullscreen.png") && md->fullscreen==false && mb) md->setFullScreen();
		if (gui.ButtonImage("butDropMan.png", md->editMode == Modeller::ED_DROPMAN) && mb) md->setEditMode(Modeller::ED_DROPMAN);
		if (gui.ButtonImage("butBigwin.png") && mu) md->setFullScene();
		if (gui.ButtonImage("butSceneRot.png", md->editMode == Modeller::ED_ROTATESCENE) && mu) md->setEditMode(Modeller::ED_ROTATESCENE);
		gui.EndGroupHorizontal("rendr.png", iconstyle.halfWidth(), iconstyle.minHeight);
	}
}

void MGui::doShapesToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu)
{
	int btop = (int)((float)iconstyle.minWidth * .34f);
	if (gui.BeginGroupVertical("butTop2.png", iconstyle.minWidth, btop)) {
		if (gui.ButtonImage("cuboid.png", md->createTool == Modeller::CT_CUBOID) && mb) md->setCreateTool(Modeller::CT_CUBOID);
		if (gui.ButtonImage("sphere.png", md->createTool == Modeller::CT_SPHERE) && mb) md->setCreateTool(Modeller::CT_SPHERE);
		if (gui.ButtonImage("cylinder.png", md->createTool == Modeller::CT_CYLINDER) && mb) md->setCreateTool(Modeller::CT_CYLINDER);
		if (gui.ButtonImage("cone.png", md->createTool == Modeller::CT_CONE) && mb) md->setCreateTool(Modeller::CT_CONE);
		if (gui.ButtonImage("tube.png", md->createTool == Modeller::CT_TUBE) && mb) md->setCreateTool(Modeller::CT_TUBE);
		if (gui.ButtonImage("t-cone.png", md->createTool == Modeller::CT_TCONE) && mb) md->setCreateTool(Modeller::CT_TCONE);
		if (gui.ButtonImage("wedge.png", md->createTool == Modeller::CT_WEDGE) && mb) md->setCreateTool(Modeller::CT_WEDGE);
		if (gui.ButtonImage("torus.png", md->createTool == Modeller::CT_TORUS) && mb) md->setCreateTool(Modeller::CT_TORUS);
		gui.renderBackIcon("butBot2.png", iconstyle.minWidth, btop);

		gui.renderBackIcon("butTop2.png", iconstyle.minWidth, btop);
		if (gui.ButtonImage("extrude.png", md->createTool == Modeller::CT_EXTRUDE) && mb) md->setCreateTool(Modeller::CT_EXTRUDE);
		if (gui.ButtonImage("lathe.png", md->createTool == Modeller::CT_LATHE) && mb) md->setCreateTool(Modeller::CT_LATHE);
		gui.renderBackIcon("butBot2.png", iconstyle.minWidth, btop);

		gui.renderBackIcon("butTop2.png", iconstyle.minWidth, btop);
		if (gui.ButtonImage("landscape.png", md->createTool == Modeller::CT_LANDSCAPE) && mb) md->setCreateTool(Modeller::CT_LANDSCAPE);
		gui.EndGroupVertical("butBot2.png", iconstyle.minWidth, btop);
	}
}

void MGui::doIMGUI(Modeller * md)
{

	//uint32_t ticks = SDL_GetTicks();
	int winWidth = md->window->getWidth();
	int winHeight = md->window->getHeight();
	int mx = md->window->mouse.x;
	int my = md->window->mouse.y;
	md->setMousePosition(mx, winHeight - my);
	bool mouseOverToolbars = mx<leftbarWidth || mx>(winWidth - rightbarWidth) || my<(topbarHeight + menuHeight) || my>winHeight - botbarHeight;

	gui.Begin();

	if (!mouseOverToolbars && gui.takenSnapshot == 2) {
		gui.drawSnapshot();
		glClear(GL_DEPTH_BUFFER_BIT);
		return;
	}

	doMenus(md);

	workWidth = winWidth - leftbarWidth - rightbarWidth;
	workHeight = winHeight - menuHeight - topbarHeight - botbarHeight;

	bool mb = md->window->mouse.LeftButton && mouseOverToolbars;
	bool mu = md->window->mouse.up && mouseOverToolbars;
	//md->setMousePosition(mx, winHeight - my);

	//Draw borders ...
	gui.renderRect(winWidth, topbarHeight);
	gui.nextLine(0);
	Pi3Cpointi wpos = gui.getPosition();
	drawBorders(md, wpos);



	//Top menu bar icons ...
	gui.setButtonStyle(bsIcons);

	gui.setPosition(wpos.x + leftbarWidth, wpos.y - topbarHeight);
	doEditToolbar(md, bsIcons, mb);

	gui.sameLine();
	doTransformToolbar(md, bsIcons, mb);

	gui.setPosition(winWidth - rightbarWidth - 150, workHeight + topbarHeight + menuHeight + 3);
	doSceneToolbar(md, bsIcons, mb, mu);

	//Left side create shape buttons ...
	gui.setPosition(wpos.x + (leftbarWidth - bsButtons.minWidth) / 2, wpos.y);
	bsButtons.sameLine = false;
	gui.setButtonStyle(bsButtons);
	doShapesToolbar(md, bsButtons, mb, mu);

	gui.setPosition(leftbarWidth, workHeight + topbarHeight + menuHeight + 3);
	gui.setButtonStyle(bsHeading);
	static double v = 5;
	gui.SliderH("Scroll", 0, 10, v, 300, 24);

	renderYellowBorder(md->currentSelView);

	//Draw xyz values
	if (md->currentView != viewInfo::INACTIVE) {
		Pi3Crecti &rect = md->views[md->currentView].viewport;
		gui.setButtonStyle(bsMenu);
		//gui.setPosition(rect.x + 5, winHeight - rect.y - 26);
		gui.setPosition(leftbarWidth + 320, workHeight + topbarHeight + menuHeight + 3);
		std::string tpos = "X:" + ftostrdp(md->currentPos.x, 2) + ", Y:" + ftostrdp(md->currentPos.y, 2) + ", Z:" + ftostrdp(md->currentPos.z, 2);
		gui.Text(tpos, 0xffffffff);
		//if (md->selectedName != "") {
		//	gui.setPosition(leftbarWidth + 720, workHeight + topbarHeight + menuHeight + 3);
		//	gui.Text(md->selectedName, 0xffffffff);
		//}
	}

	//SDL_Log("Ticks:%d",SDL_GetTicks()-ticks);
	gui.End();
	
}

std::string MGui::ftostrdp(float n, int decimalPlaces)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(decimalPlaces) << n;
	return stream.str();
}

void MGui::saveAll(Modeller * md)
{
	gui.takeSnapshot();
	std::string savefile = gui.OpenFileDialog(&bsDialog);
	if (savefile!="") md->saveFile("../", savefile);
}

