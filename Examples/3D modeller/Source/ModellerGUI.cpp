#include "ModellerGUI.h"
#include "Modeller.h"
#include "Pi3Cutils.h"

void MGui::init(loadOptions &opts, Pi3Cresource * resource, Pi3Cwindow *window, const std::string& defaultStyle)
{
	gui.init(resource, window);

	if (defaultStyle != "") guiStyle.LoadUIstyle(defaultStyle);

	// Get GUI fonts
	//resource->addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 80);
	largeFont = gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 28);
	mediumFont = gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 20);
	smallFont = gui.addFont(opts.asString("fontsPath").c_str(), opts.asString("font1").c_str(), 16);

	gui.setImagePath("assets/icons");

	// Setup GUI button styles ...
	bsMenu.font = smallFont;
	bsMenu.textColour = 0x0;
	bsMenu.buttonAlpha = 1.f;
	bsMenu.buttonColour = guiStyle.iconColour;
	bsMenu.minHeight = 32;
	bsMenu.minWidth = 70;
	bsMenu.vertGap = 0;
	bsMenu.left = 5;
	bsMenu.highlightColour = Pi3Ccolours::LightGray;

	bsHeading.font = largeFont;
	bsHeading.textColour = guiStyle.iconColour;
	bsHeading.highlightColour = guiStyle.highlight;
	bsHeading.buttonColour = guiStyle.backColour;
	bsHeading.top = 0;
	bsHeading.bottom = 0;
	bsHeading.left = 0;
	bsHeading.right = 0;

	bsIcons.font = largeFont;
	bsIcons.textColour = guiStyle.iconColour;
	bsIcons.buttonColour = guiStyle.backColour;
	bsIcons.highlightColour = guiStyle.highlight;
	bsIcons.minWidth = 32;
	bsIcons.minHeight = 32;
	bsIcons.top = 0;
	bsIcons.bottom = 0;
	bsIcons.left = 0;
	bsIcons.right = 0;
	bsIcons.horizGap = 0;
	bsIcons.sameLine = true;
	bsIcons.selectColour = guiStyle.selectColour;

	bsButtons.font = largeFont;
	bsButtons.textColour = guiStyle.iconColour;
	bsButtons.buttonColour = guiStyle.backColour;
	bsButtons.highlightColour = guiStyle.highlight;
	bsButtons.justify = Pi3Cimgui::CENTRE;
	bsButtons.align = Pi3Cimgui::CENTRE;
	bsButtons.minWidth = 40;
	bsButtons.minHeight = 40;
	bsButtons.top = 0;
	bsButtons.bottom = 0;
	bsButtons.left = 0;
	bsButtons.right = 0;
	bsButtons.vertGap = 0;
	bsButtons.selectColour = guiStyle.selectColour;
	//bsButtons.sameLine = true;

	bsItems.font = smallFont;
	bsItems.buttonAlpha = 0.8f;
	bsItems.buttonColour = guiStyle.iconColour;
	bsItems.textColour = 0x404040;
	bsItems.minWidth = 140;
	bsItems.minHeight = 40;
	bsItems.justify = Pi3Cimgui::CENTRE;
	bsItems.align = Pi3Cimgui::BOTTOM;
	bsItems.selectColour = guiStyle.selectColour;

	bsDialog.font = smallFont;
	bsDialog.buttonAlpha = 0.8f;
	bsDialog.buttonColour = guiStyle.backColour;
	bsDialog.textColour = 0x404040;
	bsDialog.minWidth = 32;
	bsDialog.minHeight = 32;
	bsDialog.justify = Pi3Cimgui::CENTRE;
	bsDialog.align = Pi3Cimgui::CENTRE;
	bsDialog.selectColour = guiStyle.selectColour;
	bsDialog.highlightColour = Pi3Ccolours::Yellow;
}

void MGui::renderBorder(uint32_t currentSelView)
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
		gui.renderThickRect(rec, 4, guiStyle.borderColour);
	}
}

Pi3Crecti MGui::getRectBottomRight()
{
	return Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX) + dbh, botbarHeight, (int)((float)workWidth * ibx)-dbh, (int)((float)workHeight * iby));
}

Pi3Crecti MGui::getRectBottomLeft()
{
	return Pi3Crecti(leftbarWidth, botbarHeight, (int)((float)workWidth * dragBarX) - dbh, (int)((float)workHeight * iby));
}

Pi3Crecti MGui::getRectTopRight()
{
	return Pi3Crecti(leftbarWidth + (int)((float)workWidth * dragBarX) + dbh, botbarHeight + (int)((float)workHeight * iby) + dbh, (int)((float)workWidth * ibx)-dbh, (int)((float)workHeight * dragBarY) - dbh * 4);
}

Pi3Crecti MGui::getRectTopLeft()
{
	return Pi3Crecti(leftbarWidth, botbarHeight + (int)((float)workHeight * iby) + dbh, (int)((float)workWidth * dragBarX) - dbh, (int)((float)workHeight * dragBarY) - dbh * 4);
}

Pi3Crecti MGui::getRectFull()
{
	return Pi3Crecti(leftbarWidth, botbarHeight, workWidth, workHeight-dbh * 3);
}

Pi3Cpointi MGui::getTopLeft()
{
	return Pi3Cpointi(leftbarWidth + dbh, topbarHeight + menuHeight + dbh*2);
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

void MGui::updateWorkArea(Modeller* md) {
	workWidth = md->window->getWidth() - leftbarWidth - rightbarWidth;
	workHeight = md->window->getHeight() - menuHeight - topbarHeight - botbarHeight;
}

void MGui::dragViewportBars(Modeller * md, Pi3Cpointi& wpos, int midht)
{
	bool mb = md->window->mouse.LeftButton;
	int mx = md->window->mouse.x;
	int my = md->window->mouse.y;
	bool notDraggingOthers = md->selectRect || draggingBarX || draggingBarY;

	updateWorkArea(md);

	if (md->fullview == viewInfo::INACTIVE) {
		//Drag bar horizontal ...
		gui.setPosition(leftbarWidth + (int)(dragBarX * (float)workWidth - dragBarThickness * 0.5f), wpos.y);

		bool touchDragBarH = gui.renderRect((int)dragBarThickness, midht, 0xffffffff);
		if (touchDragBarH || draggingBarX) md->setDragBarH(true);

		if (touchDragBarH && mb && !notDraggingOthers) {
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

		bool touchDragBarV = gui.renderRect(workWidth, (int)dragBarThickness, 0xffffffff);
		if (touchDragBarV || draggingBarY) md->setDragBarV(true);

		if (touchDragBarV && mb && !notDraggingOthers) {
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

void MGui::dragBars(Modeller * md)
{
	Pi3Cpointi wpos = getTopLeft();
	int midht = md->window->getHeight() - wpos.y - botbarHeight;
	dragViewportBars(md, wpos, midht);
	renderBorder(md->currentSelView);
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
			if (gui.MenuItem("Duplicate", "Ctrl+C")) md->duplicateSelection();
			if (gui.MenuItem("Delete", "Del")) md->deleteSelection();
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


void MGui::doEditToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu)
{
	int icw = bsIcons.halfWidth(); // (int)((float)bsIcons.minWidth*0.5f);
	int ich = bsIcons.minHeight;
	int idw = bsIcons.thirdWidth(); // (int)((float)bsIcons.minWidth*0.3f);

	//gui.renderBackIcon("rendl.png", icw, ich);
	//if (gui.Container("EditToolbar",400,32)) {
		if (gui.BeginGroupHorizontal("rendl.png", icw, ich)) {

			if (gui.ButtonImage("butNew.png") && mb && mu) md->clearScene();
			if (gui.ButtonImage("butOpen.png") && mb && mu) {}
			if (gui.ButtonImage("butSave.png") && mb && mu) saveAll(md);
			gui.renderBackIcon("butDiv.png", idw, ich);

			if (gui.ButtonImage("butArrow.png", md->editMode == Modeller::ED_SELECT) && mb && mu) md->setEditMode(Modeller::ED_SELECT);
			if (gui.ButtonImage("butMove.png", md->editMode == Modeller::ED_MOVE) && mb && mu) md->setEditMode(Modeller::ED_MOVE);
			if (gui.ButtonImage("butRotate.png", md->editMode == Modeller::ED_ROTATE) && mb && mu) md->setEditMode(Modeller::ED_ROTATE);
			if (gui.ButtonImage("butScale.png", md->editMode == Modeller::ED_SCALE) && mb && mu) md->setEditMode(Modeller::ED_SCALE);

			gui.renderBackIcon("butDiv.png", idw, ich);

			if (gui.ButtonImage("butSelectAll.png") && mb && mu) md->selectAll();
			if (gui.ButtonImage("butDelete.png") && mb && mu) md->deleteSelection();
			if (gui.ButtonImage("butCopy.png") && mb && mu) md->duplicateSelection(); //  snapshot();

			gui.renderBackIcon("butDiv.png", idw, ich);

			if (gui.ButtonImage("butGroup.png") && mb && mu) {}
			if (gui.ButtonImage("butUngroup.png") && mb && mu) {}

			gui.renderBackIcon("butDiv.png", idw, ich);

			if (gui.ButtonImage("butUndo.png") && mb && mu) {}
			if (gui.ButtonImage("butRedo.png") && mb && mu) {}

			gui.EndGroupHorizontal("rendr.png", icw, ich);
		//}
			//gui.ContainerEnd("EditToolbar");
	}
	//gui.renderBackIcon("rendr.png", icw, ich);
}

void MGui::doTransformToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu)
{
	if (gui.BeginGroupHorizontal("rendl.png", iconstyle.halfWidth(), iconstyle.minHeight)) {
		if (gui.ButtonImage("butMirror.png") && mb && mu) {}
		if (gui.ButtonImage("butFlip.png") && mb && mu) {}
		if (gui.ButtonImage("butInvertPolys.png") && mb && mu) {}
		if (gui.ButtonImage("butInvertNorms.png") && mb && mu) {}
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
	bool mouseClick = mb && mu;
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
		if (gui.ButtonImage("butLine.png", md->createTool == Modeller::CT_LINE) && mb) md->setCreateTool(Modeller::CT_LINE);
		//if (gui.ButtonImage("blocks.png", md->createTool == Modeller::CT_BLOCKS) && mb) md->setCreateTool(Modeller::CT_BLOCKS);
		gui.renderBackIcon("butBot2.png", iconstyle.minWidth, btop);

		gui.renderBackIcon("butTop2.png", iconstyle.minWidth, btop);
		if (gui.ButtonImage("landscape.png", md->createTool == Modeller::CT_LANDSCAPE) && mb) md->setCreateTool(Modeller::CT_LANDSCAPE);
		gui.EndGroupVertical("butBot2.png", iconstyle.minWidth, btop);
	}
}

void MGui::MaterialInfo(Modeller* md, Pi3Cmaterial& material, const Pi3Cpointi& p)
{
	Pi3Cmodel rect;
	rect.matrix.setz(-10.0f);
	//auto& p = gui.getCurrentPosition();
	int w = 600, h = 250, xc = p.x, yc = p.y + 10, bord = 10;
	if (xc + w > md->window->getWidth()) xc = md->window->getWidth() - w;
	float xf = (float)xc, yf = (float)yc;
	rect.createRect2D(gui.resource, vec2f(xf, yf), vec2f((float)w, (float)h), guiStyle.iconColour);
	rect.renderBasic(gui.resource);
	rect.createRect2D(gui.resource, vec2f(xf + (float)(bord), yf + (float)(bord)), vec2f((float)(h - bord * 2), (float)(h - bord * 2)), 0xffffffff, material.texRef);
	rect.renderBasic(gui.resource);
	gui.setFont(smallFont);

	int ht = h-bord/2, hs = 22;
	gui.TextAt("Name: " + ((material.name.length()>20) ? material.name.substr(0,30)+"..." : material.name), xc + h, yc + ht, 0x0); ht -= hs;
	ht -= 5;
	gui.TextAt("Diffuse colour : 0x" + Pi3Cutils::numToHexstr(material.GetColDiffuse()), xc + h, yc + ht, 0x0); ht -= hs;
	gui.TextAt("Ambient colour : 0x" + Pi3Cutils::numToHexstr(material.GetColAmbient()), xc + h, yc + ht, 0x0); ht -= hs;
	gui.TextAt("Emissive colour: 0x" + Pi3Cutils::numToHexstr(material.GetColEmissive()), xc + h, yc + ht, 0x0); ht -= hs;
	gui.TextAt("Specular colour: 0x" + Pi3Cutils::numToHexstr(material.GetColSpecular()), xc + h, yc + ht, 0x0); ht -= hs;
	if (material.texRef > 0) {
		ht -= 5;
		gui.TextAt("Texture width : " + std::to_string(material.texWidth), xc + h, yc + ht, 0x0); ht -= hs;
		gui.TextAt("Texture height: " + std::to_string(material.texHeight), xc + h, yc + ht, 0x0); ht -= hs;
		if (material.animFrames > 0) {
			ht -= 5;
			gui.TextAt("Anim frames: " + std::to_string(material.animFrames), xc + h, yc + ht, 0x0); ht -= hs;
			gui.TextAt("Anim offset: " + Pi3Cutils::ftostrdp(material.animoffset.x, 2) + "," + Pi3Cutils::ftostrdp(material.animoffset.y, 2), xc + h, yc + ht, 0x0); ht -= hs;
			gui.TextAt("Anim size  : " + Pi3Cutils::ftostrdp(material.animsize.x, 2) + "," + Pi3Cutils::ftostrdp(material.animsize.y, 2), xc + h, yc + ht, 0x0); ht -= hs;
		}
	}
}

void MGui::doMaterialsToolbar(Modeller* md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu)
{
	int icw = bsIcons.halfWidth();
	int ich = bsIcons.minHeight;
	int touchMaterial = -1;
	int selMat = -1;
	Pi3Cpointi touchMatPos;

	gui.setSameLine(true);
	gui.renderBackIcon("rendl.png", icw, ich);
	if (gui.Container("MatToolbar", 400, 32)) {
	//if (gui.BeginGroupHorizontal("rendl.png", iconstyle.halfWidth(), iconstyle.minHeight)) {
		
		std::vector<Pi3Cmaterial>& materials = md->resource->materials;
		std::vector<std::shared_ptr<Pi3Ctexture>>& textures = md->resource->textures;
		
		for (size_t m = 0; m < materials.size(); m++) {
			auto& material = materials[m];
			if (material.texRef > 0 && material.name!="default") {
				//gui.sameLine();
				if (gui.ButtonImage(material.name, material.texRef, false, 32, 32)) {
					touchMaterial = m;
					touchMatPos = gui.getCurrentPosition();
				}
			}
		}

		gui.ContainerEnd("MatToolbar");
		gui.renderBackIcon("rendr.png", icw, ich);

		Pi3Cpointi cp = gui.getCurrentPosition();
		if (touchMatPos.x > cp.x) touchMaterial = -1;
		//gui.EndGroupHorizontal("rendr.png", iconstyle.halfWidth(), iconstyle.minHeight);
	}

	if (touchMaterial >= 0) {
		auto& material = md->resource->materials[touchMaterial];
		MaterialInfo(md, material, touchMatPos);
		if (mb && mu) selMat = touchMaterial;
	}
}

bool MGui::doIMGUI(Modeller * md)
{

	//uint32_t ticks = SDL_GetTicks();
	int winWidth = md->window->getWidth();
	int winHeight = md->window->getHeight();
	int mx = md->window->mouse.x;
	int my = md->window->mouse.y;
	md->setMousePosition(mx, winHeight - my);
	bool mouseOverToolbars = gui.menuOpen || mx<leftbarWidth || mx>(winWidth - rightbarWidth) || my<(topbarHeight + menuHeight) || my>winHeight - botbarHeight;

	//SDL_Log("Menus %s", gui.menuTouch);
	
	gui.Begin();

	//If mouse is not over the toolbars, then draw a snapshot of the toolsbar and don't process them ...
	if (!mouseOverToolbars && gui.drawSnapshot()) {
		glClear(GL_DEPTH_BUFFER_BIT);
		return false;
	}
	
	doMenus(md);
	updateWorkArea(md);

	bool mb = md->window->mouse.LeftButton && mouseOverToolbars;
	bool mu = md->window->mouse.up && mouseOverToolbars;
	//md->setMousePosition(mx, winHeight - my);

	//Draw borders ...
	gui.renderRect(winWidth, topbarHeight);
	gui.nextLine(0);
	Pi3Cpointi wpos = gui.getPosition();

	int midht = winHeight - wpos.y - botbarHeight;
	gui.renderRect(leftbarWidth, midht);
	gui.movePosition(winWidth - rightbarWidth, 0);
	gui.renderRect(rightbarWidth, midht);
	gui.setPosition(0, wpos.y + midht);
	gui.renderRect(winWidth, botbarHeight);

	//Top menu bar icons ...
	gui.setButtonStyle(bsIcons);

	gui.setPosition(wpos.x + leftbarWidth, wpos.y - topbarHeight);
	doEditToolbar(md, bsIcons, mb, mu);

	gui.sameLine();
	doTransformToolbar(md, bsIcons, mb, mu);

	gui.setPosition(winWidth - rightbarWidth - 150, workHeight + topbarHeight + menuHeight + 3);
	doSceneToolbar(md, bsIcons, mb, mu);

	//Left side create shape buttons ...
	gui.setPosition(wpos.x + (leftbarWidth - bsButtons.minWidth) / 2, wpos.y);
	bsButtons.sameLine = false;
	gui.setButtonStyle(bsButtons);
	doShapesToolbar(md, bsButtons, mb, mu);

	gui.setPosition(leftbarWidth, workHeight + topbarHeight + menuHeight + 3);
	//gui.setButtonStyle(bsHeading);

	doMaterialsToolbar(md, bsIcons, mb, mu);

	//static double v = 5;
	//gui.SliderH("Scroll", 0, 10, v, 300, 24);

	//SDL_Log("Ticks:%d",SDL_GetTicks()-ticks);

	gui.End();
	gui.checkForSnapShot();

	return true;
}

void MGui::printText(const std::string& text, int x, int y, uint32_t col)
{
	gui.setButtonStyle(bsMenu);
	gui.setPosition(x, y);
	gui.Text(text, 0,0,Pi3Ccolours::White);
}

void MGui::saveAll(Modeller * md)
{
	//startSnapshot();
	//gui.takeSnapshot();
	std::string savefile = gui.OpenFileDialog(&bsDialog);
	if (savefile!="") md->saveFile("../", savefile);
}

void MGui::showProgressCB(Pi3Cwindow* window, const std::string &message, const float pcnt)
{
	//SDL_Log("Loading '%s' %d%% ...", message.c_str(), (int)pcnt);
	float mWidth = window->getWidth();
	float mHeight = window->getHeight();

	window->clear();
	gui.drawSnap();
	gui.setPosition(0, 0);
	gui.renderRect(mWidth, mHeight, Pi3Ccolours::TransparentBlack);

	Pi3Cmodel rect;
	rect.matrix.setz(-10.f);
	rect.createRect2D(gui.resource, vec2f(mWidth / 2.f - 110.f, mHeight / 2.f - 20.f), vec2f(220.f, 40.f), 0xffffffff);
	rect.renderBasic(gui.resource);
	rect.createRect2D(gui.resource, vec2f(mWidth / 2.f - 100.f, mHeight / 2.f - 10.f), vec2f(pcnt * 2.f, 20.f), 0xff00aa00);
	rect.renderBasic(gui.resource);
	rect.createRect2D(gui.resource, vec2f(mWidth / 2.f - 100.f + pcnt * 2.f, mHeight / 2.f - 10.f), vec2f((100.f - pcnt) * 2.f, 20.f), 0xff999999);
	rect.renderBasic(gui.resource);

	std::string text = message + Pi3Cutils::ftostrdp(pcnt, 0)+"%";
	printText(text, mWidth / 2.f - 110.f, mHeight / 2.f - 50.f, 0xffffffff);

	std::vector<uint32_t> eventList = window->events();

	window->SwapBuffers();
}
