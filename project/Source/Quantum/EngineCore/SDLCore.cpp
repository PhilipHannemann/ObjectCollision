#include "stdafx.h"
#include "SDLCore.h"
#include "ResourceManager.h"
#include "EngineConfig.h"
#include "FileManager.h"
#include "AScene.h"
#include <opengl_include.h>
#include <screen.h>
#include "IDebugRender.h"


	core::SDLCore::SDLCore():gui_screen(nullptr),scene(nullptr),debug_drawer(nullptr)
	{
	}


	core::SDLCore::~SDLCore()
	{
	}

	void core::SDLCore::initScreen(bool reconfigurate)
	{
		if (!reconfigurate)
		{
			if (RM_CONFIG->WindowMode.getString() == "Fullscreen")
				mainWindow = SDL_CreateWindow("Quantum", SDL_WINDOWPOS_CENTERED_DISPLAY(RM_CONFIG->WindowScreen.getInt()), SDL_WINDOWPOS_CENTERED_DISPLAY(RM_CONFIG->WindowScreen.getInt()), RM_CONFIG->WindowWidth.getInt(), RM_CONFIG->WindowHeight.getInt(), SDL_WINDOW_OPENGL | SDL_WindowFlags::SDL_WINDOW_FULLSCREEN /*| SDL_WindowFlags::SDL_WINDOW_RESIZABLE*/);
			else if (RM_CONFIG->WindowMode.getString() == "MaxWindow")
				mainWindow = SDL_CreateWindow("Quantum", SDL_WINDOWPOS_CENTERED_DISPLAY(RM_CONFIG->WindowScreen.getInt()), SDL_WINDOWPOS_CENTERED_DISPLAY(RM_CONFIG->WindowScreen.getInt()), RM_CONFIG->WindowWidth.getInt(), RM_CONFIG->WindowHeight.getInt(), SDL_WINDOW_OPENGL | SDL_WindowFlags::SDL_WINDOW_BORDERLESS | SDL_WindowFlags::SDL_WINDOW_MAXIMIZED /*| SDL_WindowFlags::SDL_WINDOW_RESIZABLE*/);
			else
				mainWindow = SDL_CreateWindow("Quantum", SDL_WINDOWPOS_CENTERED_DISPLAY(RM_CONFIG->WindowScreen.getInt()), SDL_WINDOWPOS_CENTERED_DISPLAY(RM_CONFIG->WindowScreen.getInt()), RM_CONFIG->WindowWidth.getInt(), RM_CONFIG->WindowHeight.getInt(), SDL_WINDOW_OPENGL /*| SDL_WindowFlags::SDL_WINDOW_RESIZABLE*/);
		}
		else
		{
			setTargetDisplay(RM_CONFIG->WindowScreen.getInt());
			if (RM_CONFIG->WindowMode.getString() == "Fullscreen")
			{
				setWindowSize(RM_CONFIG->WindowWidth.getInt(), RM_CONFIG->WindowHeight.getInt());
				setFullscreen();

			}
			else if (RM_CONFIG->WindowMode.getString() == "MaxWindow")
			{
				setMaximizedWindow();
			}
			else
			{
				setWindowed();
				setWindowSize(RM_CONFIG->WindowWidth.getInt(), RM_CONFIG->WindowHeight.getInt());
			}
		}
		setupViewport();
	}


	void core::SDLCore::setupViewport()
	{
		SDL_GetWindowSize(mainWindow, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);
		windowAspect = (float)windowHeight / (float)windowWidth;
	}

	void core::SDLCore::setupIcon()
	{
		SDL_Surface *surface;

		surface = SDL_LoadBMP(RM_FILE_BASE->getFilePath("ico.bmp").c_str());

		SDL_SetWindowIcon(mainWindow, surface);

		SDL_FreeSurface(surface);
	}

	void core::SDLCore::updateWindowSize()
	{
		glViewport(0, 0, windowWidth, windowHeight);
		//aspect = float(windowHeight) / float(windowWidth);
		scene->windowChanged(windowWidth, windowHeight);
		if (gui_screen != nullptr)
		{
			gui_screen->setSize(Eigen::Vector2i(windowWidth, windowHeight));
			gui_screen->resizeCallbackEvent(windowWidth, windowHeight);
		}
	}


	void core::SDLCore::init(core::AScene *start_scene)
	{

		scene = start_scene;

		/* Request opengl 4.4 context. */
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

		/* Turn on double buffering with a 24bit Z buffer.
		* You may need to change this to 16 or 32 for your system */

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
			SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
			exit(1);
		}

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		/* Erzeuge und Configuriere Fenster */
		initScreen(false);

		SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
		//SDL_SetWindowResizable();

		/* Set Window Icon */
		setupIcon();

		/* get Render Context */
		mainContext = SDL_GL_CreateContext(mainWindow);

		/* Init GLEW */
		GLenum rev;
		glewExperimental = GL_TRUE;
		rev = glewInit();

		if (GLEW_OK != rev) 
		{
			throw runtime_error("Can't init GLEW!");
		}

		/* This makes our buffer swap syncronized with the monitor's vertical refresh */
		SDL_GL_SetSwapInterval(1);
			
		/* Init Scene */
		//scene = new MySceneRenderer();
		this->updateWindowSize();
		
		gui_screen = new nanogui::Screen(Eigen::Vector2i(windowWidth, windowHeight));

		//debug_drawer = new render::DebugRender();

		scene->init(gui_screen);
	
		

		/* Make it ready */
		glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
		SDL_Event event;
		Uint64 fps_time = SDL_GetPerformanceCounter();
		int fps = 60;
		int fps_counter = 0;
		Logger::info("Initialization completed!");

		while (!quit) 
		{
			/*Needed Per-Frame init (without this it's going very strange)*/
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
			glDisable(GL_BLEND);
			glDisable(GL_STENCIL_TEST);

			/*Tick Scene*/
			scene->tick();

			/*Render Scene*/
			scene->render();

			/*Render GUI*/
			gui_screen->drawWidgets();

			/*End Frame*/
			SDL_GL_SwapWindow(mainWindow);

			/* process Events */
			while (SDL_PollEvent(&event)) 
			{
				bool handled = false;
				switch (event.type)
				{
				case SDL_QUIT:
					quit = true; handled = true;
					break;

				case SDL_MOUSEMOTION:
					handled = gui_screen->cursorPosCallbackEvent(event.motion.x, event.motion.y);
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					handled = gui_screen->mouseButtonCallbackEvent(event.button.button, event.button.type, SDL_GetModState()); 
					break;

				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_F1)
					{
						gui_screen->setVisible(!gui_screen->visible()); handled = true;
						break;
					}
				case SDL_KEYDOWN:
					handled = gui_screen->keyCallbackEvent(event.key.keysym.sym, event.key.keysym.scancode, event.key.state, SDL_GetModState()); 
					break;

				case SDL_TEXTINPUT:
					handled = gui_screen->charCallbackEvent(event.text.text); 
					break;
				
				case SDL_DROPFILE:
					handled = gui_screen->dropCallbackEvent(1, const_cast<const char**>(&event.drop.file));
					SDL_free(event.drop.file); 
					break;
					
				case SDL_MOUSEWHEEL:
					handled = gui_screen->scrollCallbackEvent(event.wheel.x, event.wheel.y); 
					break;

				case SDL_EventType::SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						SDL_GetWindowSize(mainWindow, &windowWidth, &windowHeight);
						this->updateWindowSize(); handled = true;
					}
					break;
				}

				if(!handled)
					scene->rawSDLEvent(&event);
			}

//			Logger::debug(to_string(glGetError()));

			
			/* FPS */
			Uint64 fps_time_now = SDL_GetPerformanceCounter();
			fps_counter++;

			double fps_delta = (double)((fps_time_now - fps_time) * 1000) / SDL_GetPerformanceFrequency();

			if (fps_delta > 500)
			{
				fps_delta /= (double) fps_counter;

				fps = (int) (1000.0 / fps_delta);

				SDL_SetWindowTitle(mainWindow, string("Quantum ~ " + to_string(fps) + " FPS").c_str());

				fps_time = fps_time_now;
				fps_counter = 0;
			}
			/* END */

		}

	
		//Clean up
		glUseProgram(0);
		scene->uninit();
		delete scene;
		delete debug_drawer;
		delete gui_screen;

		/* Delete our opengl context, destroy our window, and shutdown SDL */
		SDL_GL_DeleteContext(mainContext);
		SDL_DestroyWindow(mainWindow);
		SDL_Quit();

	}

	void core::SDLCore::setWindowSize(int w, int h)
	{
		SDL_SetWindowSize(mainWindow, w, h);
		windowWidth = w;
		windowHeight = h;
		this->updateWindowSize();
	}

	void core::SDLCore::setTargetDisplay(int display)
	{
		SDL_SetWindowPosition(mainWindow, SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
	}

	void core::SDLCore::setFullscreen()
	{
		SDL_SetWindowFullscreen(mainWindow, SDL_WindowFlags::SDL_WINDOW_FULLSCREEN);
	}

	void core::SDLCore::setMaximizedWindow()
	{
		//SDL_SetWindowFullscreen(mainWindow, 0);
		//SDL_SetWindowResizable(mainWindow, SDL_FALSE);
		//SDL_MaximizeWindow(mainWindow);
		SDL_SetWindowResizable(mainWindow, SDL_FALSE);
		SDL_SetWindowBordered(mainWindow, SDL_FALSE);

		int idx = SDL_GetWindowDisplayIndex(mainWindow);
		SDL_Rect bounds;
		SDL_GetDisplayBounds(idx, &bounds);
		//SDL_SetWindowResizable(g_displayWindow, SDL_FALSE);
		SDL_SetWindowBordered(mainWindow, SDL_FALSE);
		SDL_SetWindowPosition(mainWindow, bounds.x, bounds.y);
		SDL_SetWindowSize(mainWindow, bounds.w, bounds.h);

		//SDL_MaximizeWindow(mainWindow);
		int w, h;
		SDL_GetWindowSize(mainWindow, &w, &h);
		windowWidth = w;
		windowHeight = h;
		this->updateWindowSize();
	}

	void core::SDLCore::setWindowed()
	{
		//SDL_SetWindowFullscreen(mainWindow, 0);
		
		
		
		/*
		if ( (SDL_GetWindowFlags(mainWindow) & SDL_WINDOW_BORDERLESS) > 0)
		{
			SDL_SetWindowBordered(mainWindow, SDL_TRUE);
		}*/

		SDL_SetWindowBordered(mainWindow, SDL_TRUE);
		//SDL_Hack(mainWindow, SDL_TRUE);
		SDL_SetWindowResizable(mainWindow, SDL_TRUE);
	}

	void core::SDLCore::shutdown()
	{
		quit = true;
	}
