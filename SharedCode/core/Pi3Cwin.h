#pragma once

#include <stdint.h>
#include <string>
#include <functional>
#include "Pi3CGL.h"
#include "Pi3Cvector.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

// =======================================================================
// Pi3C Raspberry Pi Graphics Library
// =======================================================================
//
// The MIT License
//
// Copyright (c) 2018 Tim Skillman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =======================================================================

#define SPEEED 1000.f //milliseconds

class Pi3Cwindow {
public:

	struct options {

		options() {}
		options(const std::string &title) : title(title) {}

		std::string title;				//Windows title
		uint32_t sdlflags =				//SDL window flags
			SDL_WINDOW_OPENGL |
			SDL_WINDOW_SHOWN |
			SDL_WINDOW_RESIZABLE;
		uint32_t width = 800;			//Window/screen width
		uint32_t height = 600;			//Window/screen height
		uint32_t antialiasLevel = 0;	//Antialias samples level (0 samples (fast), 4 samples (slow - max for RPi), 8 samples (PC))
		bool fullscreen = false;		//Fullscreen mode (if true) else windowed
		uint8_t majorVsn = 2;			//OpenGL major version (2 default)
		uint8_t minorVsn = 0;			//OpenGL minor version (0 default)
		uint32_t bufferSize = 32;		//Screen buffer size (default 32)
		uint32_t depthSize = 16;		//Screen depth buffer (default 16)
		bool doubleBuffer = 1;			//Double buffer (default 1)
		bool alphaBlending = true;		//Alpha blending (default true)
		uint32_t clearColour = 0x505050;		//Screen clear colour (default 0 - black)
	};

	struct MouseParams {

		bool anyButton() { return LeftButton || MiddleButton || RightButton; }

		void resetButtons() {
			RightButton = LeftButton = MiddleButton = drag = false;
		}

		void reset() { 
			x = 0; y = 0; wheel = 0;
			up = false; wasDragging = false; 
			resetButtons();
			deltaXY = {}; XY = {}; 
		}

		int x = 0;
		int y = 0;
		int wheel = 0;
		bool drag = false;
		bool up = false;
		bool wasDragging = false;
		bool RightButton = false;
		bool MiddleButton = false;
		bool LeftButton = false;
		vec2f deltaXY = {};
		vec2f XY = {};
	};

	Pi3Cwindow();
	Pi3Cwindow(const options &winopts) { initOptions(winopts); }

	void initOptions(const options &opts);
	bool init(const char * title, const int width, const int height, const uint32_t flags, const uint8_t samples);
	void setCaption(const std::string &caption);
	void destroy();

	int getEvent() { return SDL_PollEvent(&ev); }
	std::vector<uint32_t> event(); //return a list of event handles (if any)
	void SwapBuffers();
	float getTicks() { return ticks; }
	void setClearColour(const float red, const float green, const float blue);
	void setClearColour(const uint32_t clearColour);
	uint32_t getClearColour();
	void clear();
	void setAlpha(bool v);
	int getWidth();
	int getHeight();
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	void forceMouseUp();
	void centreMouse() { SDL_WarpMouseInWindow(mWindow, mWidth / 2, mHeight / 2); }
	bool isMinimized();
	bool hasquit();
	void setquit(bool q) { quit = q; }

	//bool keydown(const SDL_Scancode key) { return (key == keyPress); }
	bool keydown() { return keyDown; }
	SDL_Scancode getKeyPress() { return keyPress; }
	SDL_Scancode getKeyUp() { return keyUp; }
	const uint8_t * getKeys() { keys = SDL_GetKeyboardState(NULL); return keys; }
	void resetKeyUp() { 
		keyUp = SDL_Scancode::SDL_SCANCODE_UNKNOWN; 
		
	}

	bool mouseUp() {
		bool u = mouse.up;
		mouse.up = false;
		return u;
	}

	std::string getInput() { return text; }
	void clearInput() { text = ""; }

	std::string getSystemInfo();
	void showErrorMessage(const std::string &heading, const std::string &error);

	//void regMouseDown(std::function<bool(int32_t, int32_t, int32_t)> ev) { evMouseDown.push_back(ev); }
	//void regMouseMove(std::function<bool(int32_t, int32_t, int32_t)> ev) { evMouseMove.push_back(ev); }
	//void regMouseUp(std::function<bool(int32_t, int32_t, int32_t)> ev) { evMouseUp.push_back(ev); }
	//void regMouseWheel(std::function<bool(int32_t, int32_t, int32_t)> ev) { evMouseWheel.push_back(ev); }
	//void regKeyPress(std::function<bool(int32_t)> ev) { evKeyPress.push_back(ev); }
	//void regResize(std::function<bool(int32_t, int32_t)> ev) { evResize.push_back(ev); }

#ifdef __WINDOWS__
	//windows specific functions
	HWND getHwnd();
	void createWindowsMenu();
#endif

	SDL_Window* handle() { return mWindow; }

	//SDL_Renderer* graphics;
	SDL_Event ev;

	MouseParams mouse;

	//Window dimensions
	int mWidth;
	int mHeight;
	float dpi = 1.f;
	float hdpi = 1.f;
	float vdpi = 1.f;
	//std::vector<Pi3CUIform> forms;

	bool keyDown = false;
	bool ctrlKey = false;
	bool shiftKey = false;
	bool resized = false;

	std::string dropfile;

	uint32_t clearColour;

private:
	const Uint8 *keys;
	SDL_Window* mWindow;
	SDL_GLContext GLcontext;
	SDL_Scancode keyPress;
	SDL_Scancode keyUp;

	std::string text;

	float ticks;
	int32_t lastTime;

	//void showProgressCB(std::string &message, float pcnt);

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
	bool quit;

	//std::vector<Pi3CUIform> *forms;

	//Event handler array for forms ...

	//std::vector<std::function<bool(int32_t, int32_t, int32_t)>> evMouseDown;
	//std::vector<std::function<bool(int32_t, int32_t, int32_t)>> evMouseMove;
	//std::vector<std::function<bool(int32_t, int32_t, int32_t)>> evMouseWheel;
	//std::vector<std::function<bool(int32_t, int32_t, int32_t)>> evMouseUp;
	//std::vector<std::function<bool(int32_t)>> evKeyPress;
	//std::vector<std::function<void()>> evDraw;
	//std::vector<std::function<void(int32_t, int32_t)>> evResize;

};
