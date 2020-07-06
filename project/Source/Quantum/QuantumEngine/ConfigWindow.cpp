#include "stdafx.h"
#include "ConfigWindow.h"
#include <EngineCore/Logger.h>
#include <string>
#include <functional>
#include <EngineCore/ResourceManager.h>
#include <EngineCore/SDLCore.h>
#include <EngineCore/EngineConfig.h>

using namespace nanogui;
using namespace std::placeholders;

void ConfigWindow::fullscreen(bool b)
{
	full = b;
	if (b)
	{
		tb_width->setEnabled(false); tb_width->setEditable(false);
		tb_height->setEnabled(false); tb_height->setEditable(false);
	}
	else
	{
		tb_width->setEnabled(true); tb_width->setEditable(true);
		tb_height->setEnabled(true); tb_height->setEditable(true);
	}
}

void ConfigWindow::save()
{
	config::EngineConfig *cfg = ResourceManager::getInstance()->getConfig();
	
	string wmode = full ? "MaxWindow" : "Window";
	bool f1 = false;

	if (cfg->WindowMode.getString() != wmode)
	{
		cfg->WindowMode.setString(wmode);

		f1 = true;
	}
	
	if (cfg->WindowScreen.getInt() != screen)
	{
		cfg->WindowScreen.setInt(screen);
	}

	if (!full && (cfg->WindowWidth.getInt() != w || cfg->WindowHeight.getInt() != h))
	{
		//ResourceManager::getInstance()->getSDLCore()->setWindowSize(w, h);
		cfg->WindowWidth.setInt(w);
		cfg->WindowHeight.setInt(h);
	}

	if (full)
		ResourceManager::getInstance()->getSDLCore()->setMaximizedWindow();
	else
	{
		ResourceManager::getInstance()->getSDLCore()->setWindowed();
		ResourceManager::getInstance()->getSDLCore()->setWindowSize(w, h);
	}

	ResourceManager::getInstance()->getSDLCore()->setTargetDisplay(screen);

	cfg->WindowWidth.setInt(w);
	cfg->load(true);

}

void ConfigWindow::shutdown()
{
	ResourceManager::getInstance()->getSDLCore()->shutdown();
}



ConfigWindow::ConfigWindow(nanogui::Screen *gui_screen):AWindow(gui_screen)
{

	config::EngineConfig *cfg = ResourceManager::getInstance()->getConfig();

	w = cfg->WindowWidth.getInt();
	h = cfg->WindowHeight.getInt();
	screen = cfg->WindowScreen.getInt();
	string wmode = cfg->WindowMode.getString();
	full = wmode == "MaxWindow";


	gui = new nanogui::FormHelper(gui_screen);
	nanogui::Window *nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), u8"Einstellungen");
	

	gui->addGroup("Fenster");
	auto cb = gui->addVariable("Vollbild", full);
	cb->setTooltip("Rahmenloses Vollbildfenster");
	
	std::function<void(bool)> cb_fullscreen = std::bind(&ConfigWindow::fullscreen, this, _1);

	cb->setCallback(cb_fullscreen);
	tb_width = gui->addVariable(u8"Breite", w);
	tb_height = gui->addVariable(u8"Höhe", h);
	gui->addVariable(u8"Bildschirm", screen)->setSpinnable(true);
	
	std::function<void()> cb_save = std::bind(&ConfigWindow::save, this);
	std::function<void()> cb_shutdown = std::bind(&ConfigWindow::shutdown, this);

	gui->addButton("Speichern", cb_save);

	gui->addGroup("");

	gui->addButton("Beenden", cb_shutdown);

	fullscreen(full);

}

ConfigWindow::~ConfigWindow()
{
	gui_screen->removeChild(gui_screen->childIndex(gui->window()));
	delete gui;
}
