#include "stdafx.h"
#include "SDLCore.h"
#include "EngineConfig.h"
#include "ResourceManager.h"
#include "FileManager.h"

using namespace core;
using namespace config;

//ResourceManager *quantumGame;
ResourceManager* ResourceManager::INSTANCE;

ResourceManager::ResourceManager() 
{
	INSTANCE = this;

	//erzeuge Pfad des Hauptverzeichnisses
	char* str = SDL_GetBasePath();
	basePath = str;
	basePath.append("../../");
	filemanagers[BASE] = new FileManager(basePath);
	SDL_free(str);

	//Init Logger
	Logger::init(0);

	//Lade Config
	config = new EngineConfig();
	config->load();

	sdlCore = new SDLCore();

	//sdlCore->init();
}

ResourceManager::~ResourceManager()
{

	delete sdlCore;

	for (int i = 0; i < numFileManagers; i++)
		delete filemanagers[i];

	delete config;


	Logger::stop();
}

int ResourceManager::startApp(core::AScene *scene)
{
	sdlCore->init(scene);
	return 0;
}
