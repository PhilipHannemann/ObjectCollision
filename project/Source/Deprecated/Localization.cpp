#include "stdafx.h"

#include "Localization.h"
#include "ResourceManager.h"
#include "PropertyLoader.h"
#include <string>
#include <unordered_map>
using namespace std;
using namespace core;

Localization::Localization()
{
}


Localization::~Localization()
{
}

void Localization::init(std::string langname)
{
	if (data != nullptr) 
	{
		delete data;
		data = nullptr;
	}

	try
	{
		//open property file
		PropertyLoader pl("content/lang/" + langname + ".lang");

		data = new unordered_map<string, string>();

		//load all entries
		PropertyLoader::PropertyEntry *pe;
		while ((pe = pl.getNextEntry()) != nullptr)
		{
			//add to hashmap
			(*data)[pe->key] = pe->value;
		}

	}
	catch (exception ex)
	{
		Logger::error("Can not load Localization file at \"" + ("content/lang/" + langname + ".lang") + "\"  Failed with error: " + ex.what());
		return;
	}
		
}

std::string Localization::get(std::string entry)
{
	if (data == nullptr)
		return entry;
	try
	{
		return data->at(entry);
	}
	catch(...)
	{
		return entry;
	}
}
