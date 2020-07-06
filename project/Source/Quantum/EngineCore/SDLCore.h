#pragma once

//#include <opengl_include.h>

namespace core
{
	class AScene;
}

namespace nanogui 
{
	class Screen;
}

namespace render
{
	class IDebugRender;
}

typedef void *SDL_GLContext;
struct SDL_Window;

namespace core
{
	class SDLCore
	{
		SDL_Window *mainWindow; /* Our window handle */
		SDL_GLContext mainContext; /* Our opengl context handle */

	private:
		void setupViewport();
		void setupIcon();
		void updateWindowSize();

		int windowWidth;
		int windowHeight;
		float windowAspect;
		bool quit = false;
		AScene *scene;
		nanogui::Screen *gui_screen;
		render::IDebugRender* debug_drawer;

	public:


		int getWindowWidth() { return windowWidth; }
		int getWindowHeight() { return windowHeight; }
		float getAspect() { return windowAspect; }

		AScene* getScene() { return scene; }

		render::IDebugRender* getDebugRender() { return debug_drawer; }
		void setDebugRender(render::IDebugRender* dr) { debug_drawer = dr; }

		void init(AScene *start_scene);
		void setWindowSize(int w, int h);
		

		void setTargetDisplay(int display);

		void setFullscreen();

		void setMaximizedWindow();

		//void setBorderlessWindow();

		void setWindowed();

		void shutdown();

		SDLCore();

		~SDLCore();
		void initScreen(bool reconfigurate);
	};
}

