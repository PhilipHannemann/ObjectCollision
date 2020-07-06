/*
src/screen.cpp -- Top-level widget and interface between NanoGUI and GLFW

A significant redesign of this code was contributed by Christian Schueller.

NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
The widget drawing code is based on the NanoVG demo application
by Mikko Mononen.

All rights reserved. Use of this source code is governed by a
BSD-style license that can be found in the LICENSE.txt file.
*/
#include "stdafx.h"

#include "screen.h"
#include "popup.h"
//#include <map>
//#include <iostream>
//#include "../Logger.h"

/* Allow enforcing the GL2 implementation of NanoVG */
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"

NAMESPACE_BEGIN(nanogui)

//std::map<GLFWwindow *, Screen *> __nanogui_screens;


/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(/*GLFWwindow *window*/) 
{
#if 0
#if defined(_WIN32)
	HWND hWnd = glfwGetWin32Window(window);
	HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	/* The following function only exists on Windows 8.1+, but we don't want to make that a dependency */
	static HRESULT(WINAPI *GetDpiForMonitor_)(HMONITOR, UINT, UINT*, UINT*) = nullptr;
	static bool GetDpiForMonitor_tried = false;

	if (!GetDpiForMonitor_tried) {
		auto shcore = LoadLibrary(TEXT("shcore"));
		if (shcore)
			GetDpiForMonitor_ = (decltype(GetDpiForMonitor_))GetProcAddress(shcore, "GetDpiForMonitor");
		GetDpiForMonitor_tried = true;
	}

	if (GetDpiForMonitor_) {
		uint32_t dpiX, dpiY;
		if (GetDpiForMonitor_(monitor, 0 /* effective DPI */, &dpiX, &dpiY) == S_OK)
			return std::round(dpiX / 96.0);
	}
	return 1.f;
#elif defined(__linux__)
	(void)window;

	/* Try to read the pixel ratio from GTK */
	FILE *fp = popen("gsettings get org.gnome.desktop.interface scaling-factor", "r");
	if (!fp)
		return 1;

	int ratio = 1;
	if (fscanf(fp, "uint32 %i", &ratio) != 1)
		return 1;

	if (pclose(fp) != 0)
		return 1;

	return ratio >= 1 ? ratio : 1;
#else
	Vector2i fbSize, size;
	glfwGetFramebufferSize(window, &fbSize[0], &fbSize[1]);
	glfwGetWindowSize(window, &size[0], &size[1]);
	return (float)fbSize[0] / (float)size[0];
#endif
#endif
	return 1.0f;
}

Screen::Screen(): Widget(nullptr), mNVGContext(nullptr), mCursor(Cursor::Arrow)
{
	memset(mCursors, 0, sizeof(SDL_Cursor *) * (int)Cursor::CursorCount);
}

Screen::Screen(const Vector2i &size) : Widget(nullptr), mNVGContext(nullptr), mCursor(Cursor::Arrow)
{
	memset(mCursors, 0, sizeof(SDL_Cursor *) * (int)Cursor::CursorCount);

	this->mSize = size;

	initialize();
}

void Screen::initialize() {

	mPixelRatio = get_pixel_ratio();

	/* Detect framebuffer properties and set up compatible NanoVG context */
	//GLint nStencilBits = 0, nSamples = 0;
	/*glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
		GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &nStencilBits);
	glGetIntegerv(GL_SAMPLES, &nSamples);*/
	/*
	int flags = 0;
	if (nStencilBits >= 8)
		flags |= NVG_STENCIL_STROKES;
	if (nSamples <= 1)
		flags |= NVG_ANTIALIAS;
#if !defined(NDEBUG)
	flags |= NVG_DEBUG;
#endif*/

	mNVGContext = nvgCreateGL3(0);
	if (mNVGContext == nullptr)
		throw std::runtime_error("Could not initialize NanoVG!");

	mVisible = true;
	setTheme(new Theme(mNVGContext));
	mMousePos = Vector2i::Zero();
	mMouseState = mModifiers = 0;
	mDragActive = false;
	mLastInteraction = SDL_GetTicks();
	mProcessEvents = true;
	

	mCursors[(int)Cursor::Arrow]	 = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	mCursors[(int)Cursor::IBeam]	 = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	mCursors[(int)Cursor::Crosshair] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	mCursors[(int)Cursor::Hand]		 = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	mCursors[(int)Cursor::HResize]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	mCursors[(int)Cursor::VResize]   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);

	//for (int i = 0; i < (int)Cursor::CursorCount; ++i)
	//	mCursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);

	/// Fixes retina display-related font rendering issue (#185)
	nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);
	nvgEndFrame(mNVGContext);
}

Screen::~Screen() 
{
	for (int i = 0; i < (int)Cursor::CursorCount; ++i) 
	{
		if (mCursors[i])
			SDL_FreeCursor(mCursors[i]);
	}
	if (mNVGContext)
		nvgDeleteGL3(mNVGContext);
}

void Screen::setVisible(bool visible) 
{
	if (mVisible != visible) 
	{
		mVisible = visible;
	}
}

void Screen::setSize(const Vector2i &size) 
{
	Widget::setSize(size);
}

void Screen::drawWidgets() {
	if (!mVisible)
		return;

#if defined(_WIN32) || defined(__linux__)
	mSize = (mSize / mPixelRatio).cast<int>();
	mFBSize = (mSize * mPixelRatio).cast<int>();
#else
	/* Recompute pixel ratio on OSX */
	if (mSize[0])
		mPixelRatio = (float)mFBSize[0] / (float)mSize[0];
#endif

	glViewport(0, 0, mFBSize[0], mFBSize[1]);
	glBindSampler(0, 0);
	nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);

	draw(mNVGContext);

	double elapsed = SDL_GetTicks() - mLastInteraction;
	
	if (elapsed > 0.5f) {
		
		const Widget *widget = findWidget(mMousePos);
		if (widget && !widget->tooltip().empty()) {
			int tooltipWidth = 150;

			float bounds[4];
			nvgFontFace(mNVGContext, "sans");
			nvgFontSize(mNVGContext, 15.0f);
			nvgTextAlign(mNVGContext, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			nvgTextLineHeight(mNVGContext, 1.1f);
			Vector2i pos = widget->absolutePosition() +
				Vector2i(widget->width() / 2, widget->height() + 10);

			nvgTextBounds(mNVGContext, pos.x(), pos.y(),
				widget->tooltip().c_str(), nullptr, bounds);
			int h = (bounds[2] - bounds[0]) / 2;
			if (h > tooltipWidth / 2) {
				nvgTextAlign(mNVGContext, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
				nvgTextBoxBounds(mNVGContext, pos.x(), pos.y(), tooltipWidth,
					widget->tooltip().c_str(), nullptr, bounds);

				h = (bounds[2] - bounds[0]) / 2;
			}
			nvgGlobalAlpha(mNVGContext,
				std::min(1.0, 2 * (elapsed - 0.5f)) * 0.8);

			nvgBeginPath(mNVGContext);
			nvgFillColor(mNVGContext, Color(0, 255));
			nvgRoundedRect(mNVGContext, bounds[0] - 4 - h, bounds[1] - 4,
				(int)(bounds[2] - bounds[0]) + 8,
				(int)(bounds[3] - bounds[1]) + 8, 3);

			int px = (int)((bounds[2] + bounds[0]) / 2) - h;
			nvgMoveTo(mNVGContext, px, bounds[1] - 10);
			nvgLineTo(mNVGContext, px + 7, bounds[1] + 1);
			nvgLineTo(mNVGContext, px - 7, bounds[1] + 1);
			nvgFill(mNVGContext);

			nvgFillColor(mNVGContext, Color(255, 255));
			nvgFontBlur(mNVGContext, 0.0f);
			nvgTextBox(mNVGContext, pos.x() - h, pos.y(), tooltipWidth,
				widget->tooltip().c_str(), nullptr);
		}
	}

	nvgEndFrame(mNVGContext);
}

bool Screen::keyboardEvent(int key, int scancode, int action, int modifiers) {
	if (mFocusPath.size() > 0) {
		for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
			if ((*it)->focused() && (*it)->keyboardEvent(key, scancode, action, modifiers))
				return true;
	}

	return false;
}

bool Screen::keyboardCharacterEvent(const char * utf8) {
	if (mFocusPath.size() > 0) {
		for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
			if ((*it)->focused() && (*it)->keyboardCharacterEvent(utf8))
				return true;
	}
	return false;
}

bool Screen::resizeEvent(const Vector2i& size) {
	if (mResizeCallback) {
		mResizeCallback(size);
		return true;
	}
	return false;
}

bool Screen::cursorPosCallbackEvent(double x, double y) {
	Vector2i p((int)x, (int)y);

#if defined(_WIN32) || defined(__linux__)
	p /= mPixelRatio;
#endif
	bool ret = false;
	mLastInteraction = SDL_GetTicks();
	try {
		p -= Vector2i(1, 2);

		if (!mDragActive) {
			Widget *widget = findWidget(p);
			if (widget != nullptr && widget->cursor() != mCursor) {
				mCursor = widget->cursor();
				SDL_SetCursor(mCursors[(int)mCursor]);
			}
		}
		else {
			ret = mDragWidget->mouseDragEvent(
				p - mDragWidget->parent()->absolutePosition(), p - mMousePos,
				mMouseState, mModifiers);
		}

		if (!ret)
			ret = mouseMotionEvent(p, p - mMousePos, mMouseState, mModifiers);

		mMousePos = p;

		return ret;
	}
	catch (const std::exception &e) {
		//core::Logger::error("NanoGui: Handling of 'cursorPosCallbackEvent' failed! Message: " + std::string(e.what()));
		return false;
	}
}

bool Screen::mouseButtonCallbackEvent(int button, int action, int modifiers) {
	mModifiers = modifiers;
	mLastInteraction = SDL_GetTicks();
	try {
		if (mFocusPath.size() > 1) {
			const Window *window =
				dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
			if (window && window->modal()) {
				if (!window->contains(mMousePos))
					return false;
			}
		}

		if (action == SDL_MOUSEBUTTONDOWN)
			mMouseState |= 1 << button;
		else
			mMouseState &= ~(1 << button);

		auto dropWidget = findWidget(mMousePos);
		if (mDragActive && action == SDL_MOUSEBUTTONUP &&
			dropWidget != mDragWidget)
			mDragWidget->mouseButtonEvent(
				mMousePos - mDragWidget->parent()->absolutePosition(), button,
				false, mModifiers);

		if (dropWidget != nullptr && dropWidget->cursor() != mCursor) {
			mCursor = dropWidget->cursor();
			//glfwSetCursor(mGLFWWindow, mCursors[(int)mCursor]);
		}

		if (action == SDL_MOUSEBUTTONDOWN && (button == SDL_BUTTON_LEFT || button == SDL_BUTTON_RIGHT)) {
			mDragWidget = findWidget(mMousePos);
			if (mDragWidget == this)
				mDragWidget = nullptr;
			mDragActive = mDragWidget != nullptr;
			if (!mDragActive)
				updateFocus(nullptr);
		}
		else {
			mDragActive = false;
			mDragWidget = nullptr;
		}

		return mouseButtonEvent(mMousePos, button, action == SDL_MOUSEBUTTONDOWN, mModifiers);
	}
	catch (const std::exception &e) {
		//core::Logger::error("NanoGui: Handling of 'mouseButtonCallbackEvent' failed! Message: " + std::string(e.what()));
		return false;
	}
}

bool Screen::keyCallbackEvent(int key, int scancode, int action, int mods) {
	mLastInteraction = SDL_GetTicks();
	try {
		return keyboardEvent(key, scancode, action, mods);
	}
	catch (const std::exception &e) {
		//core::Logger::error("NanoGui: Handling of 'keyCallbackEvent' failed! Message: " + std::string(e.what()));
		return false;
	}
}

bool Screen::charCallbackEvent(const char* utf8) {
	mLastInteraction = SDL_GetTicks();
	try {
		return keyboardCharacterEvent(utf8);
	}
	catch (const std::exception &e) {
		//core::Logger::error("NanoGui: Handling of 'charCallbackEvent' failed! Message: " + std::string(e.what()));
		return false;
	}
}

bool Screen::dropCallbackEvent(int count, const char **filenames) {
	std::vector<std::string> arg(count);
	for (int i = 0; i < count; ++i)
		arg[i] = filenames[i];
	return dropEvent(arg);
}

bool Screen::scrollCallbackEvent(double x, double y) {
	mLastInteraction = SDL_GetTicks();
	try {
		if (mFocusPath.size() > 1) {
			const Window *window =
				dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
			if (window && window->modal()) {
				if (!window->contains(mMousePos))
					return false;
			}
		}
		return scrollEvent(mMousePos, Vector2f(x, y));
	}
	catch (const std::exception &e) {
		//core::Logger::error("NanoGui: Handling of 'scrollCallbackEvent' failed! Message: " + std::string(e.what()));
		return false;
	}
}

bool Screen::resizeCallbackEvent(int x, int y) {
	Vector2i fbSize, size;

	size = Eigen::Vector2i(x, y);

#if defined(_WIN32) || defined(__linux__)
	fbSize = size * mPixelRatio;
#endif

	if (mFBSize == Vector2i(0, 0) || size == Vector2i(0, 0))
		return false;

	mFBSize = fbSize; mSize = size;
	mLastInteraction = SDL_GetTicks();

	try {
		return resizeEvent(mSize);
	}
	catch (const std::exception &e) {
		//core::Logger::error("NanoGui: Handling of 'resizeCallbackEvent' failed! Message: " + std::string(e.what()));
		return false;
	}
}

void Screen::updateFocus(Widget *widget) {
	for (auto w : mFocusPath) {
		if (!w->focused())
			continue;
		w->focusEvent(false);
	}
	mFocusPath.clear();
	Widget *window = nullptr;
	while (widget) {
		mFocusPath.push_back(widget);
		if (dynamic_cast<Window *>(widget))
			window = widget;
		widget = widget->parent();
	}
	for (auto it = mFocusPath.rbegin(); it != mFocusPath.rend(); ++it)
		(*it)->focusEvent(true);

	if (window)
		moveWindowToFront((Window *)window);
}

void Screen::disposeWindow(Window *window) {
	if (std::find(mFocusPath.begin(), mFocusPath.end(), window) != mFocusPath.end())
		mFocusPath.clear();
	if (mDragWidget == window)
		mDragWidget = nullptr;
	removeChild(window);
}

void Screen::centerWindow(Window *window) {
	if (window->size() == Vector2i::Zero()) {
		window->setSize(window->preferredSize(mNVGContext));
		window->performLayout(mNVGContext);
	}
	window->setPosition((mSize - window->size()) / 2);
}

void Screen::moveWindowToFront(Window *window) {
	mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), window), mChildren.end());
	mChildren.push_back(window);
	/* Brute force topological sort (no problem for a few windows..) */
	bool changed = false;
	do {
		size_t baseIndex = 0;
		for (size_t index = 0; index < mChildren.size(); ++index)
			if (mChildren[index] == window)
				baseIndex = index;
		changed = false;
		for (size_t index = 0; index < mChildren.size(); ++index) {
			Popup *pw = dynamic_cast<Popup *>(mChildren[index]);
			if (pw && pw->parentWindow() == window && index < baseIndex) {
				moveWindowToFront(pw);
				changed = true;
				break;
			}
		}
	} while (changed);
}

NAMESPACE_END(nanogui)
