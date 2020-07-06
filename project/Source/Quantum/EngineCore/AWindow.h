#pragma once

namespace nanogui
{
	class Screen;
	
	class AWindow 
	{
	protected:
		Screen *gui_screen;

	public:
		AWindow(Screen *gui_screen) 
		{
			this->gui_screen = gui_screen;
		}

		virtual ~AWindow(){}
	
	};

}