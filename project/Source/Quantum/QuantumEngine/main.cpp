#include "stdafx.h"
#include <string>
#include <EngineCore/ResourceManager.h>
#include <EngineCore/Logger.h>
#include <exception>
#include "MySceneRenderer.h"
#include "SceneQuantumEditor.h"

using namespace std;
using namespace core;

int normal(ResourceManager* manager)
{
	return manager->startApp(new editor::SceneQuantumEditor());
	//return manager->startApp(new MySceneRenderer());
}



int main(int argc, char *argv[]) 
{
	/*Init Core*/
	ResourceManager* manager = new ResourceManager();

	int ret = -1;

	try
	{
		/*Main Function*/
		ret = normal(manager);
	}
	catch (exception &ex)
	{
		Logger::error("Application aborted with an Exception!");
		string s = ex.what();
		Logger::error(s);
	}

	if (ret != 0)
		Logger::error("Application ended with Code: " + to_string(ret));

	/*claenup*/
	delete manager;

	return ret;
}