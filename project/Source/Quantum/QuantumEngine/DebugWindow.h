#pragma once
#include <EngineCore/AWindow.h>
#include <nanogui.h>

namespace nanogui
{
	class DebugWindow : public nanogui::AWindow
	{
	protected:
		nanogui::FormHelper *gui;
		bool spheretree = false;
		bool wireframe = false;
		bool goalpos = false;
		bool freeze = true;
		bool do_tick = false;
		int reset = false;
		Button* button_freeze = nullptr;
	public:
		bool doWireframe();
		bool showGoalpos();
		bool showSphereTree();
		bool isFrozen();
		bool shouldTick(bool test = true);
		int shouldReset(bool test = true);

		DebugWindow(nanogui::Screen *gui_screen);
		virtual ~DebugWindow();
		
	};

}
  