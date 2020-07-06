#pragma once
#include "DLLExport.h"
#include <string>
#include "Logger.h"
using namespace std;


/*Shortcuts*/
#define RM_CONFIG ResourceManager::getInstance()->getConfig()
#define RM_FILE_BASE ResourceManager::getInstance()->getFileManager(ResourceManager::FileManagers::BASE)

//forward
namespace core 
{
	class FileManager;
	class SDLCore;
	class AScene;
}

namespace config 
{
	class EngineConfig;
}


class ResourceManager
{
public:
	static int const numFileManagers = 1;
	enum FileManagers { BASE = 0 };

private:
	static ResourceManager* INSTANCE;
	string basePath;
	core::SDLCore *sdlCore;
	core::FileManager* filemanagers[numFileManagers];
	config::EngineConfig* config;

public:

	static ResourceManager* getInstance() { return INSTANCE; }

	ResourceManager();
	~ResourceManager();

	int startApp(core::AScene *scene);

	core::FileManager* getFileManager(FileManagers fm) { return filemanagers[fm]; }
	core::SDLCore *getSDLCore() { return sdlCore; }
	config::EngineConfig* getConfig() { return config; }
		
};