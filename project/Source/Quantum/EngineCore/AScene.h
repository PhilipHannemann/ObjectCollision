#pragma once
#include <SDL\SDL_events.h>
#include <glm\glm.hpp>

namespace physics { class PhysicsWorld; }
namespace nanogui { class Screen; }

namespace core
{
	class AScene
	{
	protected:
		nanogui::Screen *gui_screen;
		virtual void init() = 0;
		
		glm::ivec2 window_size;
		glm::ivec2 cursor_pos;

		inline float getAspect() 
		{
			return (float(window_size.x) / float(window_size.y));
		}

	public:
		AScene() {}
		virtual ~AScene() {}

		void init(nanogui::Screen* gui_screen) 
		{
			this->gui_screen = gui_screen;
			init();
		}	
		virtual void uninit() = 0;

		virtual void tick() = 0;
		virtual void render() = 0;

		virtual void windowChanged(int width, int height) 
		{
			window_size.x = width; 
			window_size.y = height; 
		
		}

		//virtual physics::PhysicsWorld* getPhysicWorld() = 0;

		//primary callbacks
		virtual bool rawSDLEvent(SDL_Event* event) 
		{ 
			if (event->type == SDL_MOUSEMOTION)
			{
				cursor_pos.x = event->motion.x;
				cursor_pos.y = event->motion.y;
			}

			return false;
		}

	};
}

