#include "Pi3Cwin.h"
#include "Pi3Cmatrix.h"
#include <sstream>

Pi3Cwindow::Pi3Cwindow() {
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
	mouse.reset();
	clearColour = 0x000000;
	quit = false;
}


void Pi3Cwindow::initOptions(const options &opts)
{
	mWidth = opts.width;
	mHeight = opts.height;
	if (init(opts.title.c_str(), opts.width, opts.height, opts.sdlflags | ((opts.fullscreen) ? SDL_WINDOW_FULLSCREEN : 0) | SDL_WINDOW_ALLOW_HIGHDPI, opts.antialiasLevel))
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, opts.bufferSize);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, opts.depthSize);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, opts.majorVsn);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, opts.minorVsn);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, (opts.doubleBuffer) ? 1:0);
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		GLcontext = SDL_GL_CreateContext(mWindow);
		SDL_GL_SetSwapInterval(-1);
		SDL_GL_MakeCurrent(mWindow, GLcontext);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		setAlpha(opts.alphaBlending);
		setClearColour(opts.clearColour);

#ifdef __WINDOWS__
		glewExperimental = GL_TRUE;
		GLenum glew_status = glewInit();
#endif

		//work out DPI scaling factor for screen ...
		int err = SDL_GetDisplayDPI(0, &dpi, &hdpi, &vdpi);
		if (err < 0) {
			dpi = hdpi = vdpi = 1.f; //default to 1.f if error
		}
		else {
			dpi = 1.f; // /= 72.f; //convert to font-sized 1/72 dpi
			hdpi /= 72.f;
			vdpi /= 72.f;
		}

		mMouseFocus = true;
		mKeyboardFocus = true;
	}
}

bool Pi3Cwindow::init(const char * title, int width, int height, uint32_t flags, uint8_t samples) {
	//Create window
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		SDL_Log("Unable to initialize SDL");
		return false;
	}

	if (samples > 0) {
		//Turn on anti-aliasing (before creating the SDL window) ... could be used for RPi (4 samples max) but slow!
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, samples);
	}

	mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	if (mWindow == nullptr)
	{
		//If window fails its probably because of too many samples - turn off antialiasing and try again
		SDL_Log("Couldn't initialise window - turning off antialiasing ...");
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
		mWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);  //try again without antialiasing
		if (mWindow == nullptr) {
			SDL_Log("Window error %s", SDL_GetError());		//OK - this time report an error
			return false;
		}
	}

	if (TTF_Init() == -1) {
		return false; //cant initialize TTF fonts
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		return false; //cant initialize audio
	}

	return true;
}

void Pi3Cwindow::SwapBuffers() {
	SDL_GL_SwapWindow(handle());
	int tick = SDL_GetTicks() - lastTime;
	ticks = (tick == 0) ? 1.f / SPEEED : (float)(tick) / SPEEED;
	lastTime = SDL_GetTicks();
}

std::vector<uint32_t> Pi3Cwindow::events()
{
	std::vector<uint32_t> eventList;

	while (SDL_PollEvent(&ev) != 0) {
		eventList.push_back(ev.type);

		switch (ev.type) {
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_MOUSEMOTION:
			mouse.x = ev.motion.x;
			mouse.y = ev.motion.y;
			mouse.deltaXY = vec2f((float)mouse.x, (float)mouse.y) - mouse.XY;
			mouse.XY = vec2f((float)mouse.x, (float)mouse.y);
			mouse.wasDragging = false;
			//mouse.up = false;
			mouse.drag = mouse.anyButton();
			break;
		case SDL_KEYDOWN:
			keyPress = ev.key.keysym.scancode;
			keyDown = true;
			{
				const uint8_t* keystate = getKeys();
				shiftKey = keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT];
				ctrlKey = keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL];
				//if (keystate[SDL_SCANCODE_ESCAPE]) quit = true;
			}
			break;
		case SDL_KEYUP:
			keyUp = ev.key.keysym.scancode;
			keyPress = SDL_SCANCODE_UNKNOWN;
			keyDown = false;
			shiftKey = false;
			ctrlKey = false;
			break;
		case SDL_MOUSEWHEEL:
			mouse.wheel = ev.motion.x;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (!mouse.anyButton()) {
				mouse.x = ev.button.x;
				mouse.y = ev.button.y;
				mouse.drag = false;
				//mouse.up = false;
				mouse.wasDragging = false;
				//SDL_Log("Window Mouse down");
			}
			switch (ev.button.button) {
			case SDL_BUTTON_LEFT:
				mouse.LeftButton = true;
				break;
			case SDL_BUTTON_RIGHT:
				mouse.RightButton = true;
				break;
			case SDL_BUTTON_MIDDLE:
				mouse.MiddleButton = true;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (mouse.anyButton()) {
				mouse.wasDragging = mouse.drag;
				mouse.x = ev.button.x;
				mouse.y = ev.button.y;
			}
			mouse.drag = false;
			mouse.up = true;
			switch (ev.button.button) {
			case SDL_BUTTON_LEFT: mouse.LeftButton = false; break;
			case SDL_BUTTON_RIGHT: mouse.RightButton = false; break;
			case SDL_BUTTON_MIDDLE: mouse.MiddleButton = false; break;
			}
			break;
		case SDL_TEXTINPUT:
			text = ev.text.text;
			break;
		case SDL_WINDOWEVENT:
			switch (ev.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				//Get new dimensions and repaint on window size change
				resize(ev.window.data1, ev.window.data2);
				break;

			case SDL_WINDOWEVENT_ENTER:
				mMouseFocus = true;
				break;

				//Mouse left window
			case SDL_WINDOWEVENT_LEAVE:
				mMouseFocus = false;
				break;

				//Window has keyboard focus
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				mKeyboardFocus = true;
				break;

				//Window lost keyboard focus
			case SDL_WINDOWEVENT_FOCUS_LOST:
				mKeyboardFocus = false;
				break;

				//Window minimized
			case SDL_WINDOWEVENT_MINIMIZED:
				mMinimized = true;
				break;

				//Window maxized
			case SDL_WINDOWEVENT_MAXIMIZED:
				mMinimized = false;
				break;

				//Window restored
			case SDL_WINDOWEVENT_RESTORED:
				mMinimized = false;
				break;
			}
			break;
		case SDL_DROPFILE:
			char* dropFileCh = ev.drop.file;
			dropfile = dropFileCh;
			SDL_free(dropFileCh);
			break;
		}
	}
	return eventList;
}

//void drawRect(vec3f pos, vec3f size)
//{
//	vec3f verts[4];
//	uint16_t inds[4] = { 0,1,2,3 };
//
//	verts[0] = pos;
//	verts[1] = pos + vec3f(0, size.y, 0);
//	verts[2] = pos + vec3f(size.x, 0, 0);
//	verts[3] = pos + vec3f(size.x, size.y, 0);
//
//	glDisableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, verts);
//	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, inds);
//}
//
//void Pi3Cwindow::showProgressCB(std::string &message, float pcnt)
//{
//	//SDL_Log("Loading '%s' %d%% ...", message.c_str(), (int)pcnt);
//
//	while (event()) {} //catch events
//
//	clear();
//
//	Pi3Cmatrix orthoMtx;
//	orthoMtx.SetOrtho(0, mWidth, mHeight, 0, 0.1f, 20.f);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadMatrixf(&orthoMtx.matrix[0]);
//
//	//Draw progress bar ...
//	glDisable(GL_COLOR_MATERIAL);
//	glColor4f(1.f, 1.f, 1.f, 1.f); //white RGBA
//	drawRect(vec3f(mWidth / 2.f - 110.f, mHeight / 2.f - 20.f, -2.f), vec3f(220.f, 40.f, -2.f));
//	glColor4f(0.f, 0.7f, 0.f, 1.f); //green
//	drawRect(vec3f(mWidth / 2.f - 100.f, mHeight / 2.f - 10.f, -2.f), vec3f(pcnt*2.f, 20.f, -2.f));
//	glColor4f(0.8f, 0.8f, 0.8f, 1.f); //grey
//	drawRect(vec3f(mWidth / 2.f - 100.f + pcnt * 2.f, mHeight / 2.f - 10.f, -2.f), vec3f((100.f-pcnt)*2.f, 20.f, -2.f));
//
//	SwapBuffers();
//}

void Pi3Cwindow::destroy()
{
	//SDL_DestroyRenderer(graphics);

	if (mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
		SDL_GL_DeleteContext(GLcontext);
	}
	SDL_Quit();
}

int Pi3Cwindow::getWidth()
{
	return mWidth;
}

int Pi3Cwindow::getHeight()
{
	return mHeight;
}

bool Pi3Cwindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool Pi3Cwindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

void Pi3Cwindow::forceMouseUp()
{
	mouse.resetButtons();
}

void Pi3Cwindow::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Pi3Cwindow::setAlpha(bool useAlpha)
{
	if (useAlpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} 
	else {
		glDisable(GL_BLEND);
	}
}

void Pi3Cwindow::setClearColour(const float red, const float green, const float blue)
{
	clearColour = (uint32_t)(red * 255.f) + ((uint32_t)(green * 255.f) << 8) + ((uint32_t)(blue *255.f) << 16);
	glClearColor(red, green, blue, 1.f);
}

void Pi3Cwindow::setClearColour(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	clearColour = (uint32_t)(red) + (uint32_t)(green << 8) + (uint32_t)(blue << 16);
	glClearColor(red / 255.f, green / 255.f, blue / 255.f, 1.f);
}

void Pi3Cwindow::setClearColour(const uint32_t clearColour)
{
	this->clearColour = clearColour;
	setClearColour((uint8_t)(clearColour & 255), (uint8_t)(clearColour >> 8) & 255 , (uint8_t)(clearColour >> 16) & 255 );
}

uint32_t Pi3Cwindow::getClearColour() {
	return clearColour;
}

bool Pi3Cwindow::isMinimized()
{
	return mMinimized;
}

bool Pi3Cwindow::hasquit()
{
	return quit;
}

void Pi3Cwindow::setCaption(const std::string &caption)
{
	SDL_SetWindowTitle(mWindow, caption.c_str());
}

void Pi3Cwindow::showErrorMessage(const std::string &heading, const std::string &error)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, heading.c_str(), error.c_str(), mWindow);	
}

std::string Pi3Cwindow::getSystemInfo()
{
	//Must be called after the creation of the SDL_window

	SDL_SysWMinfo info;
	std::string subsystem = "Unknown";

	SDL_VERSION(&info.version); /* initialize info structure with SDL version info */

	if (SDL_GetWindowWMInfo(mWindow, &info)) { /* the call returns true on success */
											  /* success */
		switch (info.subsystem) {
		case SDL_SYSWM_UNKNOWN:   break;
		case SDL_SYSWM_WINDOWS:   subsystem = "Microsoft Windows(TM)";  break;
		case SDL_SYSWM_X11:       subsystem = "X Window System";        break;
#if SDL_VERSION_ATLEAST(2, 0, 3)
		case SDL_SYSWM_WINRT:     subsystem = "WinRT";                  break;
#endif
		case SDL_SYSWM_DIRECTFB:  subsystem = "DirectFB";               break;
		case SDL_SYSWM_COCOA:     subsystem = "Apple OS X";             break;
		case SDL_SYSWM_UIKIT:     subsystem = "UIKit";                  break;
#if SDL_VERSION_ATLEAST(2, 0, 2)
		case SDL_SYSWM_WAYLAND:   subsystem = "Wayland";                break;
		case SDL_SYSWM_MIR:       subsystem = "Mir";                    break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 4)
		case SDL_SYSWM_ANDROID:   subsystem = "Android";                break;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
		case SDL_SYSWM_VIVANTE:   subsystem = "Vivante";                break;
		//case SDL_SYSWM_OS2:		  subsystem = "OS2";					break;
#endif
		}

		SDL_Log("This program is running SDL version %d.%d.%d on %s",
			(int)info.version.major,
			(int)info.version.minor,
			(int)info.version.patch,
			subsystem.c_str());
	}
	else {
		/* call failed */
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldn't get window information: %s", SDL_GetError());
	}

	return subsystem;
}

//int32_t Pi3Cwindow::createForm(std::string name, const char * file)
//{
//	Pi3CUIform newform(name);
//	forms.push_back(newform);
//	return forms.size() - 1;
//}

#ifdef __WINDOWS__
//windows specific functions
//HWND Pi3Cwindow::getHwnd()
//{
//	struct SDL_SysWMinfo info;
//	SDL_VERSION(&info.version);
//	SDL_GetWindowWMInfo(mWindow, &info);
//	HWND hwnd = info.info.win.window;  //NO LONGER SUPPORTED?
//	return hwnd;
//}
//
//void Pi3Cwindow::createWindowsMenu()
//{
//	HWND myWindow;
//	HMENU myMenu;
//
//	myWindow = getHwnd();
//	SDL_Log("HWND = %d", myWindow);
//	myMenu = LoadMenu(NULL, MAKEINTRESOURCE(102));
//	SetMenu(myWindow, myMenu);
//}

#endif

