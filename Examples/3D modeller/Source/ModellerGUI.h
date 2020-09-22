#pragma once
#include "Pi3Cimgui.h"
#include "Pi3CloadOptions.h"

class Modeller;  //forward declare Modeller

class MGui
{
public:

	//Functions ...
	void init(loadOptions &opts, Pi3Cresource * resource, Pi3Cwindow *window);
	void doIMGUI(Modeller * md);
	bool somethingSelected() { return gui.somethingSelected; }

	Pi3Crecti getRectBottomRight();
	Pi3Crecti getRectBottomLeft();
	Pi3Crecti getRectTopRight();
	Pi3Crecti getRectTopLeft();
	Pi3Crecti getRectFull();
	void resize() { gui.resize(); }

private:

	//Vars ...
	Pi3Cimgui gui;

	Pi3Cimgui::rectStyle bsMenu;
	Pi3Cimgui::rectStyle bsHeading;
	Pi3Cimgui::rectStyle bsIcons;
	Pi3Cimgui::rectStyle bsButtons;
	Pi3Cimgui::rectStyle bsItems;
	Pi3Cimgui::rectStyle bsDialog;

	std::string smallFont;
	std::string largeFont;
	std::string mediumFont;

	int menuHeight = 20;
	int topbarHeight = 36;
	int botbarHeight = 40;
	int leftbarWidth = 50;
	int rightbarWidth = 40;

	float dragBarX = 0.5f;
	float dragBarY = 0.5f;
	float dragBarThickness = 8.f;
	bool draggingBarX = false;
	bool draggingBarY = false;
	int dbh = (int)(dragBarThickness * 0.5f);

	//Temp work vars ...
	int workWidth = 0;
	int workHeight = 0;
	float ibx = 1.f - dragBarX;
	float iby = 1.f - dragBarY;

	void renderYellowBorder(uint32_t currentSelView);
	void dragViewportBars(Modeller * md, Pi3Cpointi& wposm, int midht);
	void saveAll(Modeller * md);
	std::string ftostrdp(float n, int decimalPlaces);
	bool draggingBars();

	void drawBorders(Modeller * md, Pi3Cpointi& wpos);
	void doMenus(Modeller * md);
	void doEditToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb);
	void doTransformToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb);
	void doSceneToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);
	void doShapesToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);
};