#include "stdafx.h"
#include "Configuration.h"
#include "ResourceManager.h"
#include "Logger.h"
#include "FileManager.h"
#include "libconfig.h"
using namespace libconfig;

namespace core {

	//only in this file; libconfig isn't needed again
	/*Config *cfg = nullptr;
	Config *cfg_write = nullptr;
	Setting *layer = nullptr;
	Setting *layer_write = nullptr;*/

	//load an Entry
	void ConfigEntry::load(Setting *layer, Setting *layer_write, const char *entry, int defaultVal)
	{
		if (layer != nullptr)
		{

			datatype = INT;
			if (layer->exists(entry))
			{
				int iv;
				if (!layer->lookupValue(entry, iv))
				{
					//eigentlich nur bei falschem Datentyp
					Logger::error("Can't load the config value for: " + string(entry));
					throw EmptyEntryException("Can't load the config!");
				}
				data = new int(iv);
			}
			else
			{
				data = new int(defaultVal);
			}
		}

		layer_write->add(entry, Setting::TypeInt) = *((int*)data);
	}

	void ConfigEntry::load(Setting *layer, Setting *layer_write, const char *entry, float defaultVal)
	{
		if (layer != nullptr)
		{
			datatype = FLOAT;
			if (layer->exists(entry))
			{
				float fv;
				if (!layer->lookupValue(entry, fv))
				{
					//eigentlich nur bei falschem Datentyp
					Logger::error("Can't load the config value for: " + string(entry));
					throw EmptyEntryException("Can't load the config!");
				}
				data = new float(fv);
			}
			else
			{
				data = new float(defaultVal);
			}
		}

		layer_write->add(entry, Setting::TypeFloat) = *((float*)data);
	}

	void ConfigEntry::load(Setting *layer, Setting *layer_write, const char *entry, string defaultVal)
	{
		if (layer != nullptr)
		{
			datatype = STRING;
			if (layer->exists(entry))
			{
				string sv;
				if (!layer->lookupValue(entry, sv))
				{
					//eigentlich nur bei falschem Datentyp
					Logger::error("Can't load the config value for: " + string(entry));
					throw EmptyEntryException("Can't load the config!");
				}
				data = new string(sv);
			}
			else
			{
				data = new string(defaultVal);
			}
		}
		layer_write->add(entry, Setting::TypeString) = *((string*)data);
	}

	void ConfigEntry::load(Setting *layer, Setting *layer_write, const char *entry, bool defaultVal)
	{
		if (layer != nullptr)
		{
			datatype = BOOL;
			if (layer->exists(entry))
			{
				bool bv;
				if (!layer->lookupValue(entry, bv))
				{
					//eigentlich nur bei falschem Datentyp
					Logger::error("Can't load the config value for: " + string(entry));
					throw EmptyEntryException("Can't load the config!");
				}
				data = new bool(bv);
			}
			else
			{
				data = new bool(defaultVal);
			}
		}
		layer_write->add(entry, Setting::TypeBoolean) = *((bool*)data);
	}


	ConfigEntry::ConfigEntry()
	{
		//Error values
		datatype = NONE;
		data = nullptr;
	}

	ConfigEntry::~ConfigEntry()
	{
		switch (datatype)
		{
		case INT:
			delete (int*)data; break;
		case FLOAT:
			delete (float*)data; break;
		case STRING:
			delete (string*)data; break;
		case BOOL:
			delete (bool*)data; break;
		}
	}

	int ConfigEntry::getInt() const
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == INT)
			return *((int*)data);

		if (datatype == FLOAT)
			return (int)getFloat();

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	float ConfigEntry::getFloat() const
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == FLOAT)
			return *((float*)data);

		if (datatype == INT)
			return (float)getInt();

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	string ConfigEntry::getString() const
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == STRING)
			return string(*((string*)data));

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	bool ConfigEntry::getBool() const
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == BOOL)
			return *((bool*)data);

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	void ConfigEntry::setInt(int i)
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == INT || datatype == FLOAT)
		{
			*((int*)data) = i;
			return;
		}

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	void ConfigEntry::setFloat(float f)
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == INT || datatype == FLOAT)
		{
			*((float*)data) = f;
			return;
		}

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	void ConfigEntry::setString(string str)
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == STRING)
		{
			*((string*)data) = str;
			return;
		}

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	void ConfigEntry::setBool(bool b)
	{
		if (data == nullptr)
			throw new EmptyEntryException("No value found for config entry!");

		if (datatype == BOOL)
		{
			*((bool*)data) = b;
			return;
		}

		throw new IncompatibleTypeException("Config entry can't parsed to this Type!");
	}

	void Configuration::layerUp()
	{
		layer = &layer->getParent();
		layer_write = &layer_write->getParent();
	}

	void Configuration::layerDown(const char * newLayer)
	{
		if (!layer->exists(newLayer))
			layer->add(newLayer, Setting::TypeGroup);

		layer = &(*layer)[newLayer];

		layer_write = &layer_write->add(newLayer, Setting::TypeGroup);
	}

	//open file with libconfig
	void Configuration::loadFile(string file)
	{
		if (cfg == nullptr)
			cfg = new Config();

		if (cfg_write == nullptr)
			cfg_write = new Config();

		try
		{
			cfg->readFile(RM_FILE_BASE->getFilePath(file).c_str());
		}
		catch (const FileIOException&)
		{
			Logger::error("Can't read config file: " + file);
		}
		catch (const ParseException &pex)
		{
			Logger::error("Config parse error at: " + string(pex.getFile()) + ":" + to_string(pex.getLine()) + " - " + string(pex.getError()));
			throw FatalException("Error while loading config!");
		}

		layer = &cfg->getRoot();
		layer_write = &cfg_write->getRoot();
	}

	void Configuration::writeFile(string file)
	{
		cfg_write->writeFile(RM_FILE_BASE->getFilePath(file).c_str());
	}

	void Configuration::cleanup()
	{
		if (cfg != nullptr)
		{
			delete cfg;
			cfg = nullptr;
		}
		if (cfg_write != nullptr)
		{
			delete cfg_write;
			cfg_write = nullptr;
		}
		layer = nullptr;
		layer_write = nullptr;
	}

	Configuration::Configuration()
	{
	}

	Configuration::~Configuration()
	{
		cleanup();
	}


}