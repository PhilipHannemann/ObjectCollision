#pragma once
#include <EngineCore/AWindow.h>
#include <nanogui.h>


class ConfigWindow : public nanogui::AWindow
{
protected:
	nanogui::FormHelper *gui;

	void fullscreen(bool b);
	void save();
	void shutdown();

	nanogui::TextBox *tb_width;
	nanogui::TextBox *tb_height;

	bool full = false;
	int w = 800;
	int h = 600;
	int screen = 0;

public:
	ConfigWindow(nanogui::Screen *gui_screen);
	virtual ~ConfigWindow();
};

