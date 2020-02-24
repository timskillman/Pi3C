#include "ModellerGUI.h"
#include "Modeller.h"


void MGui::init(loadOptions &opts, Pi3Cresource * resource, Pi3Cwindow *window)
{
	gui.init(resource, window);

	// Get GUI fonts
	//resource->addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 80);
	guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 28));
	guifonts.push_back(gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 16));

	gui.setImagePath("icons");

	uint32_t backColour = 0xcccccc;
	uint32_t white = 0xffffff;
	uint32_t highlight = 0xeeeeee;
	uint32_t selectColour = 0x00ffff;

	// Setup GUI button styles ...
	bsMenu.font = guifonts[1];
	bsMenu.textColour = 0x0;
	bsMenu.buttonAlpha = 1.f;
	bsMenu.buttonColour = white;
	bsMenu.minHeight = 32;
	bsMenu.minWidth = 70;
	bsMenu.vertGap = 0;
	bsMenu.left = 5;
	bsMenu.highlightColour = 0xc0c0c0;

	bsHeading.font = guifonts[0];
	bsHeading.textColour = white;
	bsHeading.highlightColour = 0x00ffff;
	bsHeading.buttonColour = backColour;
	bsHeading.top = 0;
	bsHeading.bottom = 0;
	bsHeading.left = 0;
	bsHeading.right = 0;

	bsIcons.font = guifonts[0];
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

	bsButtons.font = guifonts[0];
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

	bsItems.font = guifonts[1];
	bsItems.buttonAlpha = 0.8f;
	bsItems.buttonColour = white;
	bsItems.textColour = 0x404040;
	bsItems.minWidth = 140;
	bsItems.minHeight = 40;
	bsItems.justify = Pi3Cimgui::CENTRE;
	bsItems.align = Pi3Cimgui::BOTTOM;
	bsItems.selectColour = selectColour;
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
		rect.renderBasic(gui.resource, gui.resource->shaders[0]);
		rect.createRect2D(gui.resource, vec2f((float)rec.x, (float)(rec.y + rec.height) - rw), vec2f((float)rec.width, rw), col);
		rect.renderBasic(gui.resource, gui.resource->shaders[0]);
		rect.createRect2D(gui.resource, vec2f((float)rec.x, (float)rec.y), vec2f(rw, (float)rec.height), col);
		rect.renderBasic(gui.resource, gui.resource->shaders[0]);
		rect.createRect2D(gui.resource, vec2f((float)(rec.x + rec.width) - rw, (float)rec.y), vec2f(rw, (float)rec.height), col);
		rect.renderBasic(gui.resource, gui.resource->shaders[0]);
	}
}

void MGui::doIMGUI(Modeller * md)
{

	gui.Begin();

	//Menubar ...
	gui.setButtonStyle(bsMenu);
	if (gui.BeginMenuBar()) {
		if (gui.BeginMenu("File")) {
			if (gui.MenuItem("New", "Ctrl+N")) md->clearScene();
			if (gui.MenuItem("Open", "Ctrl+O")) {} // open();
			if (gui.MenuItem("Save", "Ctrl+S")) {}
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
			if (gui.MenuItem("Delete", "Del")) {}
			if (gui.MenuItem("Undo", "Ctrl+X")) {}
			if (gui.MenuItem("Redo", "Ctrl+Y")) {}
			gui.EndMenu();
		}
		gui.EndMenuBar();
	}

	int hw = md->window->getWidth();
	int hh = md->window->getHeight();
	int mx = md->window->mouse.x;
	int my = md->window->mouse.y;
	bool mb = md->window->mouse.LeftButton;
	bool mu = md->window->mouse.up;

	workWidth = hw - leftbarWidth - rightbarWidth;
	workHeight = hh - menuHeight - topbarHeight - botbarHeight;

	md->currentPos = md->views[md->currentView].calcMouseXYZ(mx, hh - my);

	//Draw borders ...
	gui.renderRect(hw, topbarHeight);
	gui.nextLine(0);
	Pi3Cpointi wpos = gui.getPosition();
	int midht = hh - wpos.y - botbarHeight;
	gui.renderRect(leftbarWidth, midht);
	gui.movePosition(hw - rightbarWidth, 0);
	gui.renderRect(rightbarWidth, midht);
	gui.setPosition(0, wpos.y + midht);
	gui.renderRect(hw, botbarHeight);

	if (md->fullview < 0) {
		//Drag bar horizontal ...
		gui.setPosition(leftbarWidth + (int)(dragBarX * (float)workWidth - dragBarThickness * 0.5f), wpos.y);

		bool touchDragBarH = gui.renderRect((int)dragBarThickness, midht);
		if (touchDragBarH || draggingBarX) md->setDragBarH(true);

		if (touchDragBarH && mb && !draggingBarX && !draggingBarY) {
			draggingBarX = true;
		}
		else if (draggingBarX && mb) {
			dragBarX = (float)(mx - leftbarWidth) / (float)workWidth;
			if (dragBarX < 0.02f) dragBarX = 0.02f;
			if (dragBarX > 0.98f) dragBarX = 0.98f;
			ibx = 1.f - dragBarX;
		}
		else {
			draggingBarX = false;
		}

		//Drag bar vertical ...
		gui.setPosition(leftbarWidth, topbarHeight + menuHeight + (int)((float)workHeight*dragBarY - dragBarThickness * 0.5f));

		bool touchDragBarV = gui.renderRect(workWidth, (int)dragBarThickness);
		if (touchDragBarV || draggingBarY) md->setDragBarV(true);

		if (touchDragBarV && mb && !draggingBarX && !draggingBarY) {
			draggingBarY = true;
		}
		else if (draggingBarY && mb) {
			dragBarY = (float)(my - topbarHeight - menuHeight) / (float)workHeight;
			if (dragBarY < 0.02f) dragBarY = 0.02f;
			if (dragBarY > 0.98f) dragBarY = 0.98f;
			iby = 1.f - dragBarY;
		}
		else {
			draggingBarY = false;
		}

		if (!(touchDragBarH || touchDragBarV || draggingBarX || draggingBarY)) {
			md->setDragBarH(false);
		}
	}

	//Top menu bar icons ...
	int icw = (int)((float)bsIcons.minWidth*0.5f);
	int ich = bsIcons.minHeight;
	int idw = (int)((float)bsIcons.minWidth*0.3f);
	gui.setButtonStyle(bsIcons);

	gui.setPosition(wpos.x + leftbarWidth, wpos.y - topbarHeight);
	//if (gui.Container("EditButton", 200, 32)) {

		gui.renderBackIcon("rendl.png", icw, ich);
		if (gui.ButtonImage("butNew.png") && mb) md->clearScene();
		if (gui.ButtonImage("butOpen.png") && mb) {}
		if (gui.ButtonImage("butSave.png") && mb) md->saveFile("D:/", "TestSave.obj");
		gui.renderBackIcon("butDiv.png", idw, ich);

		if (gui.ButtonImage("butArrow.png", md->editMode == Modeller::ED_SELECT) && mb) md->setEditMode(Modeller::ED_SELECT);
		if (gui.ButtonImage("butMove.png", md->editMode == Modeller::ED_MOVE) && mb) md->setEditMode(Modeller::ED_MOVE);
		if (gui.ButtonImage("butRotate.png", md->editMode == Modeller::ED_ROTATE) && mb) md->setEditMode(Modeller::ED_ROTATE);
		if (gui.ButtonImage("butScale.png", md->editMode == Modeller::ED_SCALE) && mb) md->setEditMode(Modeller::ED_SCALE);

		gui.renderBackIcon("butDiv.png", idw, ich);

		if (gui.ButtonImage("butSelectAll.png") && mb) md->selectAll();
		if (gui.ButtonImage("butCut.png") && mb) {}
		if (gui.ButtonImage("butPaste.png") && mb) {}
		if (gui.ButtonImage("butCopy.png") && mb) {}

		gui.renderBackIcon("butDiv.png", idw, ich);

		if (gui.ButtonImage("butUndo.png") && mb) {}
		if (gui.ButtonImage("butRedo.png") && mb) {}

		gui.renderBackIcon("rendr.png", icw, ich);
		gui.sameLine(3);
		gui.renderBackIcon("rendl.png", icw, ich);

		if (gui.ButtonImage("butMirror.png") && mb) {}
		if (gui.ButtonImage("butFlip.png") && mb) {}
		gui.renderBackIcon("rendr.png", icw, ich);

		//gui.ContainerEnd("EditButton");
	//}

	gui.setPosition(hw - rightbarWidth - 100, workHeight + topbarHeight + menuHeight + 3);
	gui.renderBackIcon("rendl.png", icw, ich);
	if (gui.ButtonImage("butBigwin.png") && mu) md->setFullScene();
	if (gui.ButtonImage("butSceneRot.png", md->editMode == Modeller::ED_ROTATESCENE) && mb) md->setEditMode(Modeller::ED_ROTATESCENE);
	gui.renderBackIcon("rendr.png", icw, ich);

	//Left side create shape buttons ...
	gui.setPosition(wpos.x + (leftbarWidth - bsButtons.minWidth) / 2, wpos.y);
	bsButtons.sameLine = false;
	gui.setButtonStyle(bsButtons);

	int btop = (int)((float)bsButtons.minWidth * .34f);
	gui.renderBackIcon("butTop2.png", bsButtons.minWidth, btop);
	if (gui.ButtonImage("cuboid.png", md->createTool == Modeller::CT_CUBOID) && mb) md->setCreateTool(Modeller::CT_CUBOID);
	if (gui.ButtonImage("sphere.png", md->createTool == Modeller::CT_SPHERE) && mb) md->setCreateTool(Modeller::CT_SPHERE);
	if (gui.ButtonImage("cylinder.png", md->createTool == Modeller::CT_CYLINDER) && mb) md->setCreateTool(Modeller::CT_CYLINDER);
	if (gui.ButtonImage("cone.png", md->createTool == Modeller::CT_CONE) && mb) md->setCreateTool(Modeller::CT_CONE);
	if (gui.ButtonImage("tube.png", md->createTool == Modeller::CT_TUBE) && mb) md->setCreateTool(Modeller::CT_TUBE);
	if (gui.ButtonImage("t-cone.png", md->createTool == Modeller::CT_TCONE) && mb) md->setCreateTool(Modeller::CT_TCONE);
	if (gui.ButtonImage("wedge.png", md->createTool == Modeller::CT_WEDGE) && mb) md->setCreateTool(Modeller::CT_WEDGE);
	if (gui.ButtonImage("torus.png", md->createTool == Modeller::CT_TORUS) && mb) md->setCreateTool(Modeller::CT_TORUS);
	gui.renderBackIcon("butBot2.png", bsButtons.minWidth, btop);
	gui.renderBackIcon("butTop2.png", bsButtons.minWidth, btop);
	if (gui.ButtonImage("extrude.png", md->createTool == Modeller::CT_EXTRUDE) && mb) md->setCreateTool(Modeller::CT_EXTRUDE);
	if (gui.ButtonImage("lathe.png", md->createTool == Modeller::CT_LATHE) && mb) md->setCreateTool(Modeller::CT_LATHE);
	gui.renderBackIcon("butBot2.png", bsButtons.minWidth, btop);
	gui.renderBackIcon("butTop2.png", bsButtons.minWidth, btop);
	if (gui.ButtonImage("landscape.png", md->createTool == Modeller::CT_LANDSCAPE) && mb) md->setCreateTool(Modeller::CT_LANDSCAPE);
	gui.renderBackIcon("butBot2.png", bsButtons.minWidth, btop);

	gui.setPosition(leftbarWidth, workHeight + topbarHeight + menuHeight + 3);
	gui.setButtonStyle(bsHeading);
	static double v = 5;
	gui.SliderH("Scroll", 0, 10, v, 300, 24);

	renderYellowBorder(md->currentSelView);

	if (md->currentView != -1) {
		Pi3Crecti &rect = md->views[md->currentView].viewport;
		gui.setButtonStyle(bsMenu);
		//gui.setPosition(rect.x + 5, hh - rect.y - 26);
		gui.setPosition(leftbarWidth + 320, workHeight + topbarHeight + menuHeight + 3);
		std::string tpos = "X:" + std::to_string(md->currentPos.x) + ", Y:" + std::to_string(md->currentPos.y) + ", Z:" + std::to_string(md->currentPos.z);
		gui.Text(tpos, 0xffffffff);
		if (md->selectedName != "") {
			gui.setPosition(leftbarWidth + 720, workHeight + topbarHeight + menuHeight + 3);
			//gui.Text(md->selectedName, 0xffffffff);
		}
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
	return Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX) + dbh, botbarHeight + (int)((float)workHeight * iby)+dbh, (int)((float)workWidth * ibx), (int)((float)workHeight * dragBarY)-dbh*3);
}

Pi3Crecti MGui::getRectTopLeft()
{
	return Pi3Crecti(leftbarWidth, botbarHeight + (int)((float)workHeight * iby) + dbh, (int)((float)workWidth * dragBarX) - dbh, (int)((float)workHeight * dragBarY) -dbh*3);
}

Pi3Crecti MGui::getRectFull()
{
	return Pi3Crecti(leftbarWidth, botbarHeight, workWidth, workHeight - dbh * 3);
}
