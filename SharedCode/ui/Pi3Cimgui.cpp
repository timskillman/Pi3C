#include "Pi3Cimgui.h"
#include "Pi3Cshapes.h"

std::string stripEndString(const std::string &str, const std::string &strip)
{
	uint32_t sz = strip.size();
	if (str.substr(str.size() - sz) == strip) return str.substr(0, str.size() - sz); else return str;
}

std::string Pi3Cimgui::addFont(const char * path, const char * fontfile, const int pointSize)
{
	std::shared_ptr<Pi3Cfont> font;
	font.reset(new Pi3Cfont(path, fontfile, pointSize));
	std::string fontname = stripEndString(fontfile, ".ttf") + "_" + std::to_string(pointSize) + "pt";
	if (font->font) {
		auto it = fonts.emplace(fontname, font);
		currentFont = it.first->second.get();
	}
	return fontname;
}

void Pi3Cimgui::setFont(const std::string &fontName)
{
	auto ff = fonts.find(fontName.c_str());
	currentFont = (ff != fonts.end()) ? ff->second.get() : nullptr;
}

void Pi3Cimgui::setButtonStyle(const rectStyle &style)
{
	currentParams = style;
	if (style.font != "") setFont(style.font);
}

Pi3Cmodel * Pi3Cimgui::createImage(const std::string &text, const std::shared_ptr<Pi3Ctexture> &texture)
{
	if (texture->isValid()) {
		Pi3Cmodel texrect;
		texrect.createRect2D(resource, vec2f(0, -1.f), vec2f(1.f, 1.f)); //Create a 1x1 sized image rectangle that can be scaled to any size
		texrect.addPicture(resource, texture);
		auto obj = imageRect.emplace(text, texrect);
		return &obj.first->second;
	}
	return nullptr;
}

Pi3Cmodel * Pi3Cimgui::createImageRect(const std::string &text, const std::shared_ptr<Pi3Ctexture> &ttex)
{
	return create2ImageRect(text, ttex);
}

Pi3Cmodel * Pi3Cimgui::create2ImageRect(const std::string &text, const std::shared_ptr<Pi3Ctexture> &ttex1, const std::shared_ptr<Pi3Ctexture> &ttex2)
{
	if (ttex1->isValid()) {
		//Create an imageRect consisting of Container, Background and 2x Images ...
		Pi3Cmodel container, texrect1, texrect2, bkgrect;

		bkgrect.createRect2D(resource);
		texrect1.createRect2D(resource);
		texrect1.addPicture(resource, ttex1);

		container.group.push_back(bkgrect);
		container.group.push_back(texrect1);
		
		if (ttex2) {
			texrect2.createRect2D(resource);
			texrect2.addPicture(resource, ttex2);
			container.group.push_back(texrect2);
		}

		auto obj = imageRect.emplace(text, container);
		return &obj.first->second;
	}
	return nullptr;
}

void Pi3Cimgui::loadImages(const std::string &path, const std::vector<std::string> &filenames)
{
	texturePath = path + "/";
	for (const auto &filename : filenames) {
		findCreateImage2(filename, IMAGE);
	}
}

Pi3Cmodel * Pi3Cimgui::findCreateImage(const std::string &str, const ButtonType type)
{
	auto findString = imageRect.find(str);
	if (findString != imageRect.end()) return &findString->second;
	std::shared_ptr<Pi3Ctexture> ttex;
	ttex.reset(new Pi3Ctexture);

	if (type == TEXT) {
		if (!currentFont) return nullptr;
		ttex.reset(currentFont->textSurface(str));
		//ttex.get()->fromTextSurface(ts);
		//SDL_FreeSurface(ts);
		//ttex = currentFont->asTexture(str, currentParams.textColour);
	}
	else {
		//ttex->loadFromFile((texturePath + str).c_str());
		ttex.reset(new Pi3Ctexture((texturePath + str).c_str(), true));
	}
	return createImageRect(str, ttex);
}

Pi3Cmodel * Pi3Cimgui::findCreateImage2(const std::string &str, const ButtonType type)
{
	auto findString = imageRect.find(str);
	if (findString != imageRect.end()) return &findString->second;
	std::shared_ptr<Pi3Ctexture> ttex;
	ttex.reset(new Pi3Ctexture);

	if (type == TEXT) {
		if (!currentFont) return nullptr;
		//SDL_Surface *ts = currentFont->textSurface(str);
		//ttex.get()->fromTextSurface(ts);
		//SDL_FreeSurface(ts);
		ttex.reset(currentFont->textSurface(str));
		//ttex = currentFont->asTexture(str, currentParams.textColour);
	}
	else {
		ttex.reset(new Pi3Ctexture((texturePath + str).c_str(), true));
	}
	return createImage(str, ttex);
}

void Pi3Cimgui::NextPos()
{
	if (currentParams.sameLine)
		nextPos = Pi3Cpointi(pos.x + size.x + currentParams.horizGap, pos.y);
	else
		nextPos = Pi3Cpointi(leftpos, pos.y - size.y - currentParams.vertGap); //next line
}

void Pi3Cimgui::setColour(Pi3Cmodel &model, uint32_t colour)
{
	model.material.SetColDiffuse(colour);
	model.material.SetColAmbient(0);
}

bool Pi3Cimgui::renderButton(Pi3Cmodel *drawObj, const int minwidth, const int minheight)
{
	if (!drawObj) return false;

	pos = nextPos;
	Pi3Cmodel &image = drawObj->group[1];
	Pi3Cmodel &rect = drawObj->group[0];

	int tw = image.material.texWidth;
	int th = image.material.texHeight;
	float aspect = (float)tw / (float)th;

	//work out button minimum size ...
	int mWidth = (minwidth == 0) ? currentParams.minWidth : minwidth; if (mWidth <= 0) mWidth = tw;
	int mHeight = (minheight == 0) ? currentParams.minHeight : minheight; if (mHeight <= 0) mHeight = th;
	size = Pi3Cpointi(mWidth, mHeight);

	//fit image into width/height of button (and retain image aspect ratio)...
	int slideWidth = mWidth - currentParams.left - currentParams.right;
	int slideHeight = mHeight - currentParams.top - currentParams.bottom;
	if (tw > slideWidth) { tw = slideWidth; th = (int)((float)tw / aspect); }
	if (th > slideHeight) { th = slideHeight; tw = (int)((float)th * aspect); }

	bool mouseTouchRect = touched(size);

	//align image into button ...
	float slx = (currentParams.justify == RIGHT) ? 1.f : (currentParams.justify == CENTRE) ? 0.5f : (currentParams.justify == LEFT) ? 0.f : currentParams.slideX;
	float sly = (currentParams.align == BOTTOM) ? 1.f : (currentParams.align == CENTRE) ? 0.5f : (currentParams.align == TOP) ? 0.f : currentParams.slideY;
	float xp = (float)(pos.x + currentParams.left) + (int)((float)(slideWidth-tw) * slx);
	float yp = (float)(pos.y - currentParams.top) - (int)((float)(slideHeight-th) * sly);

	//scale image and rectangle into button ...
	image.matrix.moveScaleXY(xp, yp - (float)th, (float)tw, (float)th);
	setColour(image, currentParams.textColour);
	image.matrix.setz(zpos);
	
	rect.matrix.moveScaleXY((float)pos.x, (float)(pos.y-size.y), (float)size.x, (float)size.y);
	rect.matrix.setz(zpos - 0.5f); 
	//rect.material.SetColDiffuse(currentParams.textColour);
	setButtonBackground(rect, mouseTouchRect);

	//render ...
	rect.renderBasic(resource);
	image.renderBasic(resource);

	NextPos();

	return mouseTouchRect;
}

bool Pi3Cimgui::renderButton2(Pi3Cmodel *drawObj, const int minwidth, const int minheight)
{
	if (!drawObj) return false;

	pos = nextPos;
	Pi3Cmodel &rect = drawObj->group[0];
	Pi3Cmodel &text = drawObj->group[1];
	Pi3Cmodel &image = drawObj->group[2];

	int tw1 = text.material.texWidth;
	int th1 = text.material.texHeight;
	float aspect1 = (float)tw1 / (float)th1;

	int tw2 = image.material.texWidth;
	int th2 = image.material.texHeight;
	float aspect2 = (float)tw2 / (float)th2;

	//work out button minimum size ...
	int mWidth = (minwidth == 0) ? currentParams.minWidth : minwidth; if (mWidth <= 0) mWidth = tw1+tw2;
	int mHeight = (minheight == 0) ? currentParams.minHeight : minheight; if (mHeight <= 0) mHeight = th1+th2;
	size = Pi3Cpointi(mWidth, mHeight);
	bool mouseTouchRect = touched(size);

	//fit image into width/height of button (and retain image aspect ratio)...
	int slideWidth = mWidth - currentParams.left - currentParams.right;
	int slideHeight = mHeight - currentParams.top - currentParams.bottom;
	if ((tw1 + tw2) > slideWidth) { 
		float f = (float)(tw1 + tw2) / (float)slideWidth;
		tw1 = (int)((float)tw1 * f); th1 = (int)((float)tw1 / aspect1);
		tw2 = (int)((float)tw2 * f); th2 = (int)((float)tw2 / aspect2); }
	if ((th1 + th2) > slideHeight) { 
		float f = (float)(th1 + th2) / (float)slideHeight;
		th1 = (int)((float)th1 * f); tw1 = (int)((float)th1 * aspect1);
		th2 = (int)((float)th2 * f); tw2 = (int)((float)th2 * aspect2); }

	//align image into button ...
	float slx = (currentParams.justify == RIGHT) ? 1.f : (currentParams.justify == CENTRE) ? 0.5f : (currentParams.justify == LEFT) ? 0.f : currentParams.slideX;
	float sly = (currentParams.align == BOTTOM) ? 1.f : (currentParams.align == CENTRE) ? 0.5f : (currentParams.align == TOP) ? 0.f : currentParams.slideY;

	float xp = (float)(pos.x + currentParams.left + (float)(slideWidth - tw1) * slx);
	float yp = (float)(pos.y - currentParams.top - (float)(slideHeight - th1) * sly);

	//scale image and rectangle into button ...
	text.matrix.moveScaleXY(xp, yp - (float)th1, (float)tw1, (float)th1);
	setColour(text, currentParams.textColour);
	text.matrix.setz(zpos);

	xp = (float)(pos.x + currentParams.left + (float)(slideWidth - tw2) * (1.f - slx));
	yp = (float)(pos.y - currentParams.top - (float)(slideHeight - th2) * (1.f - sly));

	//scale image and rectangle into button ...
	image.matrix.moveScaleXY(xp, yp - (float)th2, (float)tw2, (float)th2);
	setColour(image, currentParams.textColour);
	image.matrix.setz(zpos);

	rect.matrix.moveScaleXY((float)pos.x, (float)(pos.y - size.y), (float)size.x, (float)size.y);
	rect.matrix.setz(zpos - 0.5f);
	setButtonBackground(rect, mouseTouchRect);


	//render ...
	rect.renderBasic(resource);
	image.renderBasic(resource);
	text.renderBasic(resource);

	NextPos();

	return mouseTouchRect;
}


void Pi3Cimgui::setButtonBackground(Pi3Cmodel &rect, const bool mouseTouchRect)
{
	bool menuSelect = (menuTouch == "" || (menuTouch != "" && selectingMenu));
	if (currentParams.selected)
		rect.material.SetColDiffuse(currentParams.selectColour);
	else
		rect.material.SetColDiffuse((mouseTouchRect && currentParams.selectable && menuSelect) ? currentParams.highlightColour : currentParams.buttonColour);
	rect.material.SetColAmbient(0);
	rect.material.alpha = currentParams.buttonAlpha;
}

bool Pi3Cimgui::renderRect(const int width, const int height, uint32_t colour)
{
	pos = nextPos;
	size = Pi3Cpointi(width, height);
	bool mouseTouchRect = touched(size);
	renderRectAt(size, pos, colour);
	return mouseTouchRect;
}

void Pi3Cimgui::renderRectAt(const Pi3Cpointi& size, const Pi3Cpointi& pos, const uint32_t colour)
{
	Pi3Cmodel rect;
	rect.createRect2D(resource, vec2f((float)pos.x, (float)(pos.y - size.y)), vec2f((float)(size.x), (float)(size.y)));
	rect.matrix.setz(zpos-1.f);
	//setButtonBackground(rect, mouseTouchRect);
	if (colour != 0) rect.material.alpha = (float)((colour>>24) & 255)/255.f;
	setColour(rect, (colour == 0) ? currentParams.buttonColour : colour);
	rect.renderBasic(resource);
}

Pi3Cpointi Pi3Cimgui::calcImageSize(int tw, int th, const int minwidth, const int minheight, bool squash)
{
	float aspect = (float)tw / (float)th;

	//work out button minimum size ...
	size.x = (minwidth == 0) ? currentParams.minWidth : minwidth; if (size.x <= 0) size.x = tw;
	size.y = (minheight == 0) ? currentParams.minHeight : minheight; if (size.y <= 0) size.y = th;
	if (squash) return size;

	//fit image into width/height of button (and retain image aspect ratio)...
	int innerWidth = size.x - currentParams.left - currentParams.right;
	int innerHeight = size.y - currentParams.top - currentParams.bottom;
	if (tw > innerWidth) { tw = innerWidth; th = (int)((float)tw / aspect); }
	if (th > innerHeight) { th = innerHeight; tw = (int)((float)th * aspect); }
	return Pi3Cpointi(tw, th);
}

bool Pi3Cimgui::renderIconImage(Pi3Cmodel *image, const int width, const int height, bool squash, const uint32_t colour)
{
	if (!image) return false;
	pos = nextPos;
	//size = Pi3Cpointi(width, height);
	Pi3Cpointi wh = calcImageSize(image->material.texWidth, image->material.texHeight, width, height, squash);

	bool mouseTouchRect = touched(size);
	image->matrix.move(vec3f((float)pos.x, (float)(pos.y-wh.y - currentParams.top),zpos));
	image->matrix.SetScales(vec3f((float)wh.x, (float)wh.y, 1.f));
	setColour(*image, colour);
	image->renderBasic(resource);
	NextPos();
	return mouseTouchRect;
}

bool Pi3Cimgui::renderIcon(const std::string &str, const int minwidth, const int minheight)
{
	return renderIconImage(findCreateImage2(str, IMAGE), minwidth, minheight, true, currentParams.textColour);
}

bool Pi3Cimgui::renderText(const std::string &str, const int minwidth, const int minheight, uint32_t colour)
{
	return renderIconImage(findCreateImage2(str, TEXT), minwidth, minheight, false, (colour==0) ? currentParams.textColour : colour);
}

bool Pi3Cimgui::renderBackIcon(const std::string &str, const int minwidth, const int minheight)
{
	return renderIconImage(findCreateImage2(str, IMAGE), minwidth, minheight, true, currentParams.buttonColour);
}

bool Pi3Cimgui::touched(const Pi3Cpointi &size)
{
	int mx = window->mouse.x; // / window->dpi;
	int my = window->getHeight() - window->mouse.y; // / window->dpi;
	return (mx > pos.x && mx < (pos.x + size.x)) && (my < pos.y && my >(pos.y - size.y));
}

bool Pi3Cimgui::Container(const std::string &name, const int maxwidth, const int maxheight)
{
	Pi3Crecti rect;
	int w = maxwidth;
	int h = maxheight;

	auto cit = containers.find(name);
	if (cit == containers.end()) {
		containerStruct c;
		c.pos = pos;
		containers.insert({ name, c });
	}
	else {
		containerStruct &c = cit->second;
		if (c.initialised) {
			size = c.size;

			if (w > 0 && size.x - c.scroll.x > w) size.x = w;
			if (h > 0 && size.y - c.scroll.y > h) size.y = h;

			rect.x = pos.x + currentParams.left;
			rect.y = pos.y - currentParams.top - size.y;
			rect.width = size.x - currentParams.left - currentParams.right;
			rect.height = size.y - currentParams.top - currentParams.bottom;
			glScissor(rect.x, rect.y, rect.width, rect.height);

			int mx = window->mouse.x;
			int my = window->getHeight() - window->mouse.y;

			if (rect.touch(mx, my)) {
				if (mx < rect.x + 10 && c.scroll.x>0) c.scroll.x--;
				if (mx > (rect.x + rect.width - 10) && c.scroll.x < (rect.width + maxwidth)) c.scroll.x++;
				if (my < rect.y + 10 && c.scroll.y>0) c.scroll.y--;
				if (my > (rect.y + rect.height - 10) && c.scroll.y < (rect.height-maxheight)) c.scroll.y++;
			}
			nextPos.x -= c.scroll.x;
			nextPos.y += c.scroll.y;
		}
	}
	
	nextPos += Pi3Cpointi(currentParams.left, currentParams.top);
	glEnable(GL_SCISSOR_TEST);
	
	return true;
	//return mouseTouchRect;
}

bool Pi3Cimgui::ContainerEnd(const std::string &name)
{
	auto cit = containers.find(name);
	if (cit == containers.end()) {
		//Error! Can't find Container!
	}
	else {
		containerStruct &c = cit->second;
		if (!c.initialised) {
			c.size.x = nextPos.x - c.pos.x;
			c.size.y = (nextPos.y + size.y) - c.pos.y;
			c.initialised = true;
		}
	}

	glDisable(GL_SCISSOR_TEST);
	return false;
}

bool Pi3Cimgui::ButtonText(const std::string &str, const bool selected, const int minWidth, const int minHeight)
{
	currentParams.selected = selected;
	bool touched = renderButton(findCreateImage(str, TEXT), minWidth, minHeight);
	bool clicked = touched && window->mouse.LeftButton;
	currentParams.selected = false;
	if (touched) somethingSelected = true;
	return clicked;
}

bool Pi3Cimgui::ButtonImage(const std::string &img, const bool selected, const int minWidth, const int minHeight)
{
	currentParams.selected = selected;
	bool touched = renderButton(findCreateImage(img, IMAGE), minWidth, minHeight);
	bool clicked = touched && window->mouse.LeftButton;
	currentParams.selected = false;
	if (touched) somethingSelected = true;
	return clicked;
}

bool Pi3Cimgui::SliderH(const std::string &text, const double from, const double too, double &value, const int minwidth, const int minheight)
{
	int mWidth = (minwidth == 0) ? currentParams.minWidth : minwidth; // / dpi;
	int mHeight = (minheight == 0) ? currentParams.minHeight : minheight; // / dpi;

	int arrowWidth = mHeight / 2;
	int handleWidth = mHeight;
	int hw = handleWidth / 2;

	int slideWidth = mWidth - (arrowWidth*2 + handleWidth);
	double step = (too - from) / (double)slideWidth;

	pos = nextPos;
	size.y = mHeight;
	bool mb = window->mouse.LeftButton;
	bool mouseTouchRect = touched(Pi3Cpointi(currentParams.minWidth, currentParams.minHeight));

	if (renderBackIcon("scrollLeft.png", arrowWidth, mHeight) && mb && (value - step > from)) value -= step;

	sameLine(0);
	int left = nextPos.x;
	if (renderRect(slideWidth + handleWidth, mHeight) && mb) {
		int dx = window->mouse.x - left;
		if (dx < hw) dx = hw; else if (dx > slideWidth + hw) dx = slideWidth + hw;
		value = ((double)(dx - hw) / (double)slideWidth)*(too - from);
	}

	nextPos.x += (int)(((double)slideWidth / (too - from))*value);
	if (renderIcon("handle.png", handleWidth, mHeight) && mb) {};

	nextPos.x = left+ slideWidth; pos.x = nextPos.x;
	sameLine(0);
	if (renderBackIcon("scrollRight.png", arrowWidth, mHeight) && mb && (value + step < too)) value += step;
	//nextLine(3);
	NextPos();

	if (mouseTouchRect) somethingSelected = true;
	return mouseTouchRect;
}

bool Pi3Cimgui::SliderDoubleV(const std::string &text, const double from, const double too, double &value, const int minwidth, const int minheight)
{
	//float slideHeight = currentParams.minHeight - 24.f;
	//double step = (too - from) / (double)slideHeight;
	bool mouseTouchRect = touched(Pi3Cpointi(currentParams.minWidth, currentParams.minHeight));
	//if (renderIcon("uarrow.png", 5.f) && window->mouse.LeftButton && (value - step > from)) value -= step;
	//if (renderIcon("handle.png", slideHeight) && window->mouse.LeftButton) value = (window->mouse.y - pos.y) / slideHeight * (too - from);
	//if (renderIcon("darrow.png", 5.f) && window->mouse.LeftButton && (value + step < too)) value += step;
	return mouseTouchRect;
}

bool Pi3Cimgui::SliderFloat(const std::string &text, const float from, const float too, float &value)
{
	return false;
}

bool Pi3Cimgui::SliderInt(const std::string &text, const int32_t from, const int32_t too, int32_t &value)
{
	return false;
}

bool Pi3Cimgui::BeginGroupHorizontal(const std::string &icon, const int icw, const int ich)
{
	if (icon!="") renderBackIcon(icon, icw, ich);
	return true;
}

bool Pi3Cimgui::BeginGroupVertical(const std::string &icon, const int icw, const int ich)
{
	if (icon != "") renderBackIcon(icon, icw, ich);
	return true;
}

void Pi3Cimgui::EndGroupHorizontal(const std::string &icon, const int icw, const int ich)
{
	if (icon != "") renderBackIcon(icon, icw, ich);
}

void Pi3Cimgui::EndGroupVertical(const std::string &icon, const int icw, const int ich)
{
	if (icon != "") renderBackIcon(icon, icw, ich);
}

bool Pi3Cimgui::InputText(const std::string &text, std::string &input, const int minWidth, const int minHeight, uint32_t colour)
{
	//Help from http://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php

	int mWidth = (minWidth == 0) ? currentParams.minWidth : minWidth; // / dpi;
	int mHeight = (minHeight == 0) ? currentParams.minHeight : minHeight; // / dpi;

	bool touch = renderRect(mWidth, mHeight, Pi3Ccolours::White);
	bool clicked = touch && window->mouse.LeftButton;
	bool finished = false;

	if (clicked) {
		thisTextField = textID;
		if (!textEditing) {
			SDL_StartTextInput();
			textEditing = true;
			somethingSelected = true;
		}
		//calc nearest position to mouse pointer ...
	}

	if (textID == thisTextField) {
		//animate caret at text position...
		int textWidth, textHeight;
		currentFont->getStringSize(input, textWidth, textHeight);
		renderRectAt(Pi3Cpointi(3, mHeight - 4), Pi3Cpointi(pos.x + textWidth, pos.y - 2), Pi3Ccolours::Red);

		//add/remove characters from input ...
		if (textEditing) {
			if (window->getKeyUp() == SDL_SCANCODE_BACKSPACE)
			{
				if (input.length() > 0) input.pop_back();
				window->resetKeyUp();
			}
			else if (window->getKeyUp() == SDLK_c && SDL_GetModState() & KMOD_CTRL)
			{
				SDL_SetClipboardText(input.c_str());
			}
			//Handle paste
			else if (window->getKeyUp() == SDLK_v && SDL_GetModState() & KMOD_CTRL)
			{
				input = input + SDL_GetClipboardText();
			}
			else if (window->getKeyUp() == SDL_SCANCODE_RETURN) {
				finished = true;
			}

			std::string text = window->getInput();
			if (text != "" && !((text[0] == 'c' || text[0] == 'C') && (text[0] == 'v' || text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
				input += text;
				window->clearInput();
			}
		}
		
	}

	//uint32_t col = (colour == 0) ? currentParams.textColour | 0xff000000 : colour;
	if (input != "") {
		resource->renderText(resource->lettersRef, currentFont, input, vec3f((float)pos.x, (float)pos.y, zpos), 8000.f, (colour==0) ? currentParams.textColour : colour); //(float)mWidth
	}
	textID++;

	NextPos();

	return finished;
}

bool Pi3Cimgui::InputFloat(const std::string &text, const float from, const float too, float &value)
{
	return false;
}

bool Pi3Cimgui::InputDouble(const std::string &text, const double from, const double too, double &value)
{
	return false;
}

bool Pi3Cimgui::InputInt(const std::string &text, const int32_t from, const int32_t too, int32_t &value)
{
	return false;
}

bool Pi3Cimgui::Checkbox(const std::string &text, bool &value, const uint32_t group)
{
	return false;
}

bool Pi3Cimgui::Radio(const std::string &text, bool &value)
{
	return false;
}

bool Pi3Cimgui::Combo(const std::string &text, uint32_t &currentSelection, const std::vector<std::string> &values)
{
	return false;
}

bool Pi3Cimgui::ComboIcons(const std::string &text, const std::string &images, uint32_t &currentSelection, const std::vector<std::string> &values)
{
	return false;
}

bool Pi3Cimgui::ListBox(const std::string &text, uint32_t &currentItem, const std::vector<std::string> &values, const int minwidth, const int minheight, const ListBoxFlags flags)
{
	bool mb = window->mouse.LeftButton;

	if (renderRect(minwidth, minheight) && mb) {

	}

	return false;
}

bool Pi3Cimgui::Label(const std::string &text, const int minwidth, const int minheight)
{
	return renderText(text, minwidth, minheight);
}

bool Pi3Cimgui::Text(const std::string &text, const int minwidth, const int minheight, const uint32_t colour)
{
	if (text == "") return false;
	uint32_t col = (colour == 0) ? currentParams.textColour | 0xff000000 : colour;
	pos = nextPos;
	Pi3Crect textSize = resource->renderText(resource->lettersRef, currentFont, text, vec3f((float)nextPos.x, (float)nextPos.y, zpos), 8000.f, col);
	size.x = ((uint32_t)textSize.width > minwidth) ? (uint32_t)textSize.width : minwidth;
	size.y = ((uint32_t)textSize.height > minheight) ? (uint32_t)textSize.height : minheight;
	NextPos();
	return false;
}

bool Pi3Cimgui::TextArea(std::string &text, const int minwidth, const int minheight)
{
	pos = nextPos;
	Pi3Crect textSize = resource->renderText(resource->lettersRef, currentFont, text, vec3f(pos.x, pos.y, zpos), minwidth);
	size.x = ((uint32_t)textSize.width > minwidth) ? (uint32_t)textSize.width : minwidth;
	size.y = ((uint32_t)textSize.height > minheight) ? (uint32_t)textSize.height : minheight;
	NextPos();
	return false;
}

void Pi3Cimgui::setPosition(const int x, const int y)
{ 
	pos = Pi3Cpointi(x, window->getHeight() - y); // / window->dpi - y);
	nextPos = pos; 
	leftpos = x; 
}

void Pi3Cimgui::movePosition(const int x, const int y)
{ 
	pos += Pi3Cpointi(x, -y);
	nextPos += Pi3Cpointi(x, -y);
	leftpos += x; 
}

Pi3Cpointi Pi3Cimgui::getPosition()
{
	return Pi3Cpointi(nextPos.x, window->getHeight() - nextPos.y);
}

void Pi3Cimgui::Begin() {
	glClear(GL_DEPTH_BUFFER_BIT);
	//TODO: IMGUI should have it's own basic shader and projection matrix ...
	if (GUIshader>=0) resource->shaders[GUIshader].setProjectionMatrix(projMatrix);
	setPosition(0, 0);
	groupCo = 0;
	textID = 0;
	if (!window->mouse.LeftButton) somethingSelected = false;
}

void Pi3Cimgui::End()
{
	if (!takeSnapshot) snapshot();
}

bool Pi3Cimgui::BeginMenuBar()
{
	currentParams.selectable = false;
	menuNextItemPos = Pi3Cpointi(pos.x + currentParams.horizGap, pos.y);
	bool touch = ButtonText(" ", false, 4000);
	currentParams.selectable = true;
	menuEndPos = Pi3Cpointi(pos.x, pos.y - size.y - currentParams.vertGap);
	nextPos = Pi3Cpointi(leftpos, pos.y);
	if (touch) menuTouch = "";
	return true;
}

void Pi3Cimgui::EndMenuBar()
{
	nextPos = menuEndPos;
	leftpos = menuEndPos.x;
	zpos = -10.f;
	selectingMenu = false;
}

bool Pi3Cimgui::BeginMenu(const std::string &menuHeading)
{
	nextPos = menuNextItemPos;
	leftpos = menuNextItemPos.x;
	bool touch = ButtonText(menuHeading, false, currentParams.minWidth, currentParams.minHeight);
	menuNextItemPos = Pi3Cpointi(pos.x + size.x + currentParams.horizGap, pos.y);
	if (touch) menuTouch = menuHeading;
	return (menuTouch== menuHeading || (touch && window->mouse.LeftButton));
}

void Pi3Cimgui::EndMenu()
{

}

bool Pi3Cimgui::MenuItem(const std::string &menuItem, const std::string &itemHotkey, const int minWidth, const int minheight)
{
	zpos = -5.f;
	selectingMenu = true;
	bool touch = ButtonText(menuItem, false, minWidth, currentParams.minHeight);
	sameLine(0);
	nextPos.x -= 70;
	renderText(itemHotkey,70, currentParams.minHeight);
	//renderIcon("scrollLeft.png");
	//bool touch2 = Button(itemHotkey, TEXT, 70.f);
	nextLine(0);
	bool clicked = (touch && window->mouse.LeftButton);
	if (clicked) { menuTouch = ""; somethingSelected = true; }
	return clicked;
}

void Pi3Cimgui::snapshot()
{
	std::vector<uint8_t> snap;
	Pi3Cutils::snapShot(Pi3Crecti(0, 0, window->getWidth(), window->getHeight()), snap);

	std::shared_ptr<Pi3Ctexture> snaptex;
	snaptex.reset(new Pi3Ctexture(window->getWidth(), window->getHeight(), snap, 4));

	snapShotPic.deleteTexture(resource); //delete any previous snapshot texture
	snapShotPic.createRect2D(resource, vec2f(0, 0), vec2f(window->getWidth(), window->getHeight()));
	snapShotPic.addPicture(resource, snaptex);

	takeSnapshot = true;
}

std::string Pi3Cimgui::OpenFileDialog(const rectStyle * style)
{   
	rectStyle prevstyle = currentParams;
	if (style) setButtonStyle(*style);

	std::string inptext = "";
	bool finished = false;
	snapShotPic.matrix.setz(-30.f);
	setGaps(3, 3);
	currentParams.textColour = Pi3Ccolours::Black;
	
	while (!window->hasquit() && !finished) {

		window->clear();
		std::vector<uint32_t> events = window->event();

		snapShotPic.renderBasic(resource);

		Begin();
		renderRect(window->getWidth(), window->getHeight(), Pi3Ccolours::TransparentBlack);

		setPosition(100, 100);

		renderText("Enter Filename:", 200, 0, Pi3Ccolours::White);
		nextLine();

		finished = InputText("Filename", inptext, 203, 0, Pi3Ccolours::Black);
		//sameLine();
		if (ButtonText("Save",false,80)) finished = true;
		sameLine();
		if (ButtonText("Cancel",false,120)) {
			inptext = ""; finished = true;
		}

		window->SwapBuffers();
	}
	setButtonStyle(prevstyle);
	return inptext;
}