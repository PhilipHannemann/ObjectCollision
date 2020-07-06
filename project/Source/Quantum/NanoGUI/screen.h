/*
    nanogui/screen.h -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include "widget.h"

struct SDL_Cursor;

NAMESPACE_BEGIN(nanogui)

/**
 * \class Screen screen.h nanogui/screen.h
 *
 * \brief Represents a display surface (i.e. a full-screen or windowed GLFW window)
 * and forms the root element of a hierarchy of nanogui widgets.
 */
class NANOGUI_EXPORT Screen : public Widget {
    friend class Widget;
    friend class Window;
public:
   
    Screen(const Vector2i &size);

    /// Release all resources
    virtual ~Screen();

    /// Set the top-level window visibility (no effect on full-screen windows)
    void setVisible(bool visible);

    /// Set window size
    void setSize(const Vector2i& size);

    /// Return the ratio between pixel and device coordinates (e.g. >= 2 on Mac Retina displays)
    float pixelRatio() const { return mPixelRatio; }

    /// Handle a file drop event
    virtual bool dropEvent(const std::vector<std::string> & /* filenames */) { return false; /* To be overridden */ }

    /// Default keyboard event handler
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    /// Text input event handler: codepoint is native endian UTF-32 format
    virtual bool keyboardCharacterEvent(const char *utf8);

    /// Window resize event handler
    virtual bool resizeEvent(const Vector2i& size);

    /// Set the resize callback
    std::function<void(Vector2i)> resizeCallback() const { return mResizeCallback; }
    void setResizeCallback(const std::function<void(Vector2i)> &callback) { mResizeCallback = callback; }

    /// Return the last observed mouse position value
    Vector2i mousePos() const { return mMousePos; }

    /// Return a pointer to the underlying nanoVG draw context
    NVGcontext *nvgContext() { return mNVGContext; }

    using Widget::performLayout;

    /// Compute the layout of all widgets
    void performLayout() {
        Widget::performLayout(mNVGContext);
    }

public:
    /********* API for applications which manage GLFW themselves *********/

    /**
     * \brief Default constructor
     *
     * Performs no initialization at all. Use this if the application is
     * responsible for setting up GLFW, OpenGL, etc.
     *
     * In this case, override \ref Screen and call \ref initalize() with a
     * pointer to an existing \c GLFWwindow instance
     *
     * You will also be responsible in this case to deliver GLFW callbacks
     * to the appropriate callback event handlers below
     */
    Screen();

    /// Initialize the \ref Screen
    void initialize();

    /* Event handlers */
    bool cursorPosCallbackEvent(double x, double y);
    bool mouseButtonCallbackEvent(int button, int action, int modifiers);
    bool keyCallbackEvent(int key, int scancode, int action, int mods);
	bool charCallbackEvent(const char * utf8);
    bool dropCallbackEvent(int count, const char **filenames);
    bool scrollCallbackEvent(double x, double y);
    bool resizeCallbackEvent(int width, int height);

    /* Internal helper functions */
	void updateFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);
    void drawWidgets();

protected:
    NVGcontext *mNVGContext;
    SDL_Cursor *mCursors[(int) Cursor::CursorCount];
    Cursor mCursor;
    std::vector<Widget *> mFocusPath;
    Vector2i mFBSize;
    float mPixelRatio;
    int mMouseState, mModifiers;
    Vector2i mMousePos;
    bool mDragActive;
    Widget *mDragWidget = nullptr;
    double mLastInteraction;
    bool mProcessEvents;
    //bool mFullscreen;
    std::function<void(Vector2i)> mResizeCallback;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
