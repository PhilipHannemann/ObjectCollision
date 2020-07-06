#pragma once

#include "Configuration.h"

namespace config
{
	class EngineConfig : public core::Configuration
	{
	
	public:

		//List all Enrtys here
		core::ConfigEntry LogLevel;

		core::ConfigEntry WindowWidth;
		core::ConfigEntry WindowHeight;
		core::ConfigEntry WindowScreen;
		core::ConfigEntry WindowMode;		//Window/MaxWindow/Fullscreen

		core::ConfigEntry RealtimePhysic;

	
		virtual void load(bool rewrite = false)
		{
			loadFile("client.cfg");

			this->rewrite = rewrite;

			//Load all Entrys here
			loadE(LogLevel, "logLevel", 3);	core::Logger::setLogLevel(LogLevel.getInt());  //now there is the LogLevel; set It!
			layerDown("graphics");
			layerDown("window");
			loadE(WindowWidth, "width", 1920);
			loadE(WindowHeight, "height", 1080);
			loadE(WindowScreen, "screen", 0);
			loadE(WindowMode, "mode", string("MaxWindow"));		//Window/MaxWindow/Fullscreen

			layerUp();
			layerUp();

			layerDown("physic");
			loadE(RealtimePhysic, "realtime", true);

			writeFile("client.cfg");
			cleanup();

			core::Logger::info("Config loaded successful");
		}
	
	};

}

