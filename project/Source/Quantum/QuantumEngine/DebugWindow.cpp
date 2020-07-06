#include "stdafx.h"
#include "DebugWindow.h"

//#include "Logger.h"
//#include <string>
//#include <functional>
//#include "ResourceManager.h"
//#include "SDLCore.h"
//#include "EngineConfig.h"

using namespace nanogui;
using namespace std::placeholders;

namespace nanogui
{
	DebugWindow::DebugWindow(nanogui::Screen * gui_screen) :AWindow(gui_screen)
	{
		gui = new nanogui::FormHelper(gui_screen);
		nanogui::Window *nanoguiWindow = gui->addWindow(Eigen::Vector2i(170, 10), u8"              Debug              ");

		gui->addGroup("Debug Draw");
		auto pv = gui->addVariable("Wireframe", wireframe);
		pv->setTooltip("Wireframe Ansicht der Szene");

		auto lv = gui->addVariable("Goalpos", goalpos);
		lv->setTooltip("Anzeigen der Zielpositionen");

		auto ld = gui->addVariable(u8"Kollisionshüllen", spheretree);
		lv->setTooltip(u8"Anzeigen der Kollisionshüllen auf unterster Ebene");

		//auto  ld = gui->addVariable("No Depth", no_depth);
		//ld->setTooltip(u8"über Mesh rendern");

		auto time = gui->addGroup("World");
		nanogui::Layout* horizontal = new nanogui::BoxLayout(Orientation::Horizontal, Alignment::Middle, 50, 3);
		time->setLayout(horizontal);
		Button* b;
		b = new Button(time, "");
		b->setIcon(this->freeze ? 0x2016 : 0x25B6); //pause : start
		b->setCallback([this, b] {
			this->freeze = !this->freeze;
			b->setIcon(this->freeze ? 0x2016 : 0x25B6);
		});
		this->button_freeze = b;

		b = new Button(time, "");
		b->setIcon(0x23ED);
		b->setCallback([this, b] {
			this->do_tick = true;
		});  

		gui->addGroup("");
		b = gui->addButton(u8"Szene: Würfel", [this] {
			this->reset = 1;
			this->freeze = true;
			this->button_freeze->setIcon(this->freeze ? 0x2016 : 0x25B6);
		});

		gui->addGroup("");
		b = gui->addButton("Szene: Affe", [this] {
			this->reset = 2;
			this->freeze = true;
			this->button_freeze->setIcon(this->freeze ? 0x2016 : 0x25B6);
		});

		gui->addGroup("");
		b = gui->addButton("Szene: Billard", [this] {
			this->reset = 3;
			this->freeze = true;
			this->button_freeze->setIcon(this->freeze ? 0x2016 : 0x25B6);
		});
	}

	DebugWindow::~DebugWindow()
	{
		gui_screen->removeChild(gui_screen->childIndex(gui->window()));
		delete gui;
	}

	bool DebugWindow::doWireframe()
	{
		return this->wireframe;
	}

	bool DebugWindow::showGoalpos()
	{
		return this->goalpos;
	}

	bool DebugWindow::showSphereTree()
	{
		return spheretree;
	}

	bool DebugWindow::isFrozen()
	{
		return this->freeze;
	}

	bool DebugWindow::shouldTick(bool test)
	{
		bool old = this->do_tick;
		if (!test)
			this->do_tick = false;

		return old;
	}

	int DebugWindow::shouldReset(bool test)
	{
		int old = this->reset;
		if (!test)
			this->reset = 0;

		return old;
	}
}
