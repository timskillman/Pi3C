#pragma once

#include "Pi3Cwin.h"
#include "Pi3Cresource.h"
#include "Pi3Cmodel.h"
#include "Pi3Cvector.h"
#include "Pi3Cfont.h"
#include "Pi3Ctexture.h"
#include "Pi3Crect.h"
#include "Pi3Cutils.h"
#include "Pi3Ccolours.h"
#include "Pi3Cutils.h"
#include <string>
#include <vector>

class Pi3Cimgui {
public:
	Pi3Cimgui() {}
	Pi3Cimgui(Pi3Cresource * resource, Pi3Cwindow *window) : resource(resource), window(window) {}
	~Pi3Cimgui() {}

	enum Alignment { LEFT, CENTRE, RIGHT, TOP, BOTTOM };
	enum ButtonType { TEXT, IMAGE };
	enum ListBoxFlags { SCROLL_VERTICAL, SCROLL_HORIZONTAL };

	struct containerStruct {
		bool initialised = false;
		Pi3Cpointi pos;
		Pi3Cpointi size;
		Pi3Cpointi scroll;
		Pi3Cpointi endPos;
	};

	struct rectStyle {
		uint32_t textColour = Pi3Ccolours::White;
		uint32_t buttonColour = Pi3Ccolours::DarkGray;
		uint32_t imageColour = Pi3Ccolours::White;
		uint32_t highlightColour = Pi3Ccolours::Gray;
		uint32_t selectColour = Pi3Ccolours::Yellow;
		Alignment justify = LEFT;
		Alignment align = TOP;
		float buttonAlpha = 1.f;
		int left = 5;
		int right = 5;
		int top = 2;
		int bottom = 2;
		int minWidth = 0;
		int minHeight = 0;
		int vertGap = 3;
		int horizGap = 3;
		float slideX = 0.f;
		float slideY = 0.f;
		bool sameLine = false;
		bool selected = false;
		bool selectable = true;
		bool disabled = false;
		std::string font;

		float halfWidth() { return minWidth * 0.5f; }
		float thirdWidth() { return minWidth * 0.3f; }

	};

	void init(Pi3Cresource * resource, Pi3Cwindow *window) { this->resource = resource; this->window = window; }

	void setPosition(const int x, const int y);
	void movePosition(const int x, const int y);
	Pi3Cpointi getPosition();

	void setFont(const std::string &fontName);

	std::shared_ptr<Pi3Cfont> getFont(const std::string &fontName) {
		auto fi = fonts.find(fontName);
		if (fi != fonts.end()) return fi->second;
		return nullptr;
	}

	void setSelectColour(const uint32_t selectColour) { this->currentParams.selectColour = selectColour; }
	void setButtonColour(const uint32_t buttonColour) { this->currentParams.buttonColour = buttonColour; }
	void setHighlightColour(const uint32_t buttonColour) { this->currentParams.highlightColour = currentParams.highlightColour; }
	void setTextColour(const uint32_t textColour) { this->currentParams.textColour = textColour; }
	void setColour(Pi3Cmodel& model, uint32_t colour);
	void setButtonStyle(const rectStyle &style);
	void setGaps(const int hgap, const int vgap) { currentParams.horizGap = hgap; currentParams.vertGap = vgap; }
	void Indent();
	void Unindent();

	//Image loaders ...
	void loadImages(const std::string &path, const std::vector<std::string> &filenames);
	int32_t addImage(const std::string &name, Pi3Ctexture *image);
	void setImagePath(const std::string &path) { texturePath = path+"/"; }

	void selectButton(const bool select) { currentParams.selected = select; }
	void setAlpha(float alpha) { currentParams.buttonAlpha = alpha; }
	void setPadding(const int left, const int right, const int top, const int bottom) { currentParams.left = left; currentParams.right = right; currentParams.top = top; currentParams.bottom = bottom; }
	void setTopPadding(const int top) { currentParams.top = top; }
	void sameLine(const int gap = 3) { nextPos = Pi3Cpointi(pos.x + size.x + gap, pos.y); }
	void nextLine(const int gap = 3) { nextPos = Pi3Cpointi(leftpos, pos.y - size.y - gap); }
	std::string addFont(const char * path, const char * fontfile, const int pointSize);

	void Begin();
	void End();

	//Widgets ...
	bool Container(const std::string &name, const int minwidth = 0, const int minheight = 0);
	bool ContainerEnd(const std::string &name);
	bool Label(const std::string &text, const int minwidth = 0, const int minheight = 0);										/* Render static label (doesn't change)*/
	bool Text(const std::string& text, const uint32_t colour = 0);
	bool Text(const std::string &text, const int minwidth = 0, const int minheight = 0, const uint32_t colour = 0);		/* Render dynamic text */
	bool TextArea(std::string &text, const int minwidth, const int minheight);
	bool ButtonText(const std::string &text, const bool selected = false, const int minwidth = 0, const int minheight = 0);
	bool ButtonImage(const std::string &img, const bool selected = false, const int minWidth = 0, const int minHeight = 0);
	bool SliderH(const std::string &text, const double from, const double too, double &value, const int minwidth = 0, const int minheight = 0);
	bool SliderDoubleV(const std::string &text, const double from, const double too, double &value, const int minwidth = 0, const int minheight = 0);
	bool SliderFloat(const std::string &text, const float from, const float too, float &value);
	bool SliderInt(const std::string &text, const int32_t from, const int32_t too, int32_t &value);
	bool InputText(const std::string &text, std::string &input, const int minWidth = 0, const int minHeight = 0, uint32_t colour = 0);
	bool InputFloat(const std::string &text, const float from, const float too, float &value);
	bool InputDouble(const std::string &text, const double from, const double too, double &value);
	bool InputInt(const std::string &text, const int32_t from, const int32_t too, int32_t &value);
	bool Checkbox(const std::string &text, bool &value, const uint32_t group);
	bool Radio(const std::string &text, bool &value);
	bool Combo(const std::string &text, uint32_t &currentSelection, const std::vector<std::string> &values);
	bool ComboIcons(const std::string &text, const std::string &images, uint32_t &currentSelection, const std::vector<std::string> &values);
	bool ListBox(const std::string &text, uint32_t &currentSelection, const std::vector<std::string> &values, const int minwidth = 0.f, const int minheight = 0.f, const ListBoxFlags flags = ListBoxFlags::SCROLL_VERTICAL);

	std::string OpenFileDialog(const rectStyle * style = nullptr);

	bool BeginGroupHorizontal(const std::string &icon = "", const int icw = 0, const int ich = 0);
	bool BeginGroupVertical(const std::string &icon = "", const int icw = 0, const int ich = 0);
	void EndGroupHorizontal(const std::string &icon = "", const int icw = 0, const int ich = 0);
	void EndGroupVertical(const std::string &icon = "", const int icw = 0, const int ich = 0);

	void startGroupRect();
	void updateGroupSize(int32_t x, int32_t y);
	void endGroupRect();

	void startContainer(const std::string& name);

	void startSnapshot() { takenSnapshot = 1; }
	void stopSnapshot() { takenSnapshot = 0; }
	void takeSnapshot();
	void drawSnapshot();

	//Menu bar widget ...
	bool BeginMenuBar(const std::string& name);
	void EndMenuBar(const std::string& name);
	bool BeginMenu(const std::string &menuHeading);
	bool MenuItem(const std::string &menuItem, const std::string &itemHotkey, const int minWidth = 200.f, const int minheight = 24.f);
	void EndMenu();

	bool renderRect(const int minwidth, const int minheight, uint32_t colour = 0);
	void renderRectAt(const Pi3Cpointi& size, const Pi3Cpointi& pos, const uint32_t colour);
	bool renderIcon(const std::string &str, const int minwidth = 0, const int minheight = 0);
	bool renderText(const std::string &str, const int minwidth = 0, const int minheight = 0, uint32_t colour = 0);
	bool renderBackIcon(const std::string &str, const int minwidth = 0, const int minheight = 0);
	void resize();

	bool somethingSelected = false;
	int takenSnapshot = 0;

	Pi3Cresource * resource = nullptr;

private:

	bool touched(const Pi3Cpointi &size);
	bool renderIconImage(Pi3Cmodel *drawObj, const int minwidth, const int minheight, bool squash, const uint32_t colour);
	bool renderButton(Pi3Cmodel * drawObj, const int minwidth = 0, const int minheight = 0);
	bool renderButton2(Pi3Cmodel *drawObj, const int minwidth = 0, const int minheight = 0);
	void NextPos();
	Pi3Cpointi calcImageSize(int tw, int th, const int minwidth, const int minheight, bool squash = false);
	void setButtonBackground(Pi3Cmodel &rect, const bool mouseTouchRect);


	Pi3Cmodel * findCreateImage(const std::string &str, const ButtonType type);
	Pi3Cmodel * findCreateImage2(const std::string &str, const ButtonType type);
	Pi3Cmodel * createImage(const std::string &text, const std::shared_ptr<Pi3Ctexture> &ttex);
	Pi3Cmodel * createImageRect(const std::string &text, const std::shared_ptr<Pi3Ctexture> &ttex);
	Pi3Cmodel * create2ImageRect(const std::string &text, const std::shared_ptr<Pi3Ctexture> &ttex1, const std::shared_ptr<Pi3Ctexture> &ttex2 = nullptr);

	Pi3Cwindow *window = nullptr;

	std::map<std::string, Pi3Cmodel> imageRect{};				//library of font models with images (both text and image)
	std::map<std::string, std::shared_ptr<Pi3Cfont>> fonts{};	//library of fonts
	Pi3Cfont *currentFont = nullptr;
	std::string texturePath;

	rectStyle currentParams;	//Current style of button colours/gaps etc.. - can be replaced entirely or partly with setting functions

	//size, positions of widgets ...
	Pi3Cpointi pos{ 0,0 };			//Last position widget was drawn at (topleft corner)
	Pi3Cpointi size{ 20, 10 };		//Last size widget was drawn as (width/height)
	Pi3Cpointi nextPos{ 0,0 };		//Next position to draw widget
	float zpos = -10.f;			//Uses Z-buffer to place windows/menus in front of each other
	int leftpos = 0;		//returns to this leftmost position (used for indent etc..)

	//size, positions of menus ...
	Pi3Cpointi menuEndPos;			//Menu item next position
	Pi3Cpointi menuNextItemPos;		//Menubar heading next position
	std::string menuTouch;		//Currently touch menu heading (for drawing correct menuItem groups)
	bool selectingMenu = false;	//Ignore other hovers/selections while menu is open with this

	bool textEditing = false;
	bool getOpenFile = false;

	int32_t textID = -1;
	int32_t thisTextField = -1;
	
	Pi3Cmodel snapShotPic;
	

	//std::vector<containerStruct> containers;
	std::map<std::string, containerStruct> containers;

	int GUIshader = -1;
	Pi3Cmatrix projMatrix;

	std::vector<Pi3Crecti> groupSize;
	int groupCo = 0;
};
