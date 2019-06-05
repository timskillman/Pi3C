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

private:

	//Vars ...
	Pi3Cimgui gui;

	Pi3Cimgui::rectStyle bsMenu;
	Pi3Cimgui::rectStyle bsHeading;
	Pi3Cimgui::rectStyle bsIcons;
	Pi3Cimgui::rectStyle bsButtons;
	Pi3Cimgui::rectStyle bsItems;

	std::vector<std::string> guifonts;

	int menuHeight = 20;
	int topbarHeight = 36;
	int botbarHeight = 40;
	int leftbarWidth = 50;
	int rightbarWidth = 20;

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

};