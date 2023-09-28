#pragma once
#include "Pi3Cimgui.h"
#include "Pi3CloadOptions.h"
#include "UIStyles.h"

class Modeller;  //forward declare Modeller

class MGui
{
public:

	//Functions ...
	void Init(loadOptions &opts, Pi3Cresource * resource, Pi3Cwindow *window, const std::string& defaultStyle);
	bool DoIMGUI(Modeller * md);
	bool somethingSelected() { return gui.somethingSelected; }
	void startSnapshot() { gui.startSnapshot(); }
	void takeSnapshot() { gui.takenSnapshot = gui.SH_TAKESHOT; gui.takeSnapshot(); }
	//void stopSnapshot() { gui.stopSnapshot(); }
	void DragBars(Modeller* md);
	void PrintText(const std::string& text, int x, int y, uint32_t col = 0);
	void ShowProgressCB(Pi3Cwindow* window, const std::string& message, const float pcnt);

	Pi3Crecti GetRectBottomRight();
	Pi3Crecti GetRectBottomLeft();
	Pi3Crecti GetRectTopRight();
	Pi3Crecti GetRectTopLeft();
	Pi3Crecti GetRectFull();
	Pi3Cpointi GetTopLeft();

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

	UI_Style guiStyle;

	//uint32_t backColour = Pi3Ccolours::Raspberry; // 0xcccccc;
	//uint32_t iconColour = Pi3Ccolours::Linen;
	//uint32_t selectColour = Pi3Ccolours::LightCoral; // 0x00ffff;
	//uint32_t highlight = Pi3Ccolours::Salmon;
	//uint32_t borderColour = Pi3Ccolours::Red;

	std::string smallFont;
	std::string largeFont;
	std::string mediumFont;

	int menuHeight = 20;
	int topbarHeight = 36;
	int botbarHeight = 42;
	int leftbarWidth = 50;
	int rightbarWidth = 40;

	float dragBarX = 0.5f;
	float dragBarY = 0.5f;
	float dragBarThickness = 8.f;
	bool draggingBarX = false;
	bool draggingBarY = false;
	int dbh = (int)(dragBarThickness * 0.5f);

	bool fastRender = false; // fast draw into 3D views - no GUI rendering

	//Temp work vars ...
	int workWidth = 0;
	int workHeight = 0;
	float ibx = 1.f - dragBarX;
	float iby = 1.f - dragBarY;

	void UpdateWorkArea(Modeller* md);

	void RenderBorder(uint32_t currentSelView);
	void DragViewportBars(Modeller * md, Pi3Cpointi& wposm, int midht);
	void SaveAll(Modeller * md);
	bool DraggingBars();


	void DoMenus(Modeller * md);
	void DoEditToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);
	void DoTransformToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);
	void DoSceneToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);
	void DoShapesToolbar(Modeller * md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);
	void DoMaterialsToolbar(Modeller* md, Pi3Cimgui::rectStyle& iconstyle, bool mb, bool mu);

	void MaterialInfo(Modeller* md, Pi3Cmaterial& material, const Pi3Cpointi& p);
};