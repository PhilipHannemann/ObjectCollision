#pragma once

#include "BaseExceptions.h"
#include "Logger.h"
#include <string>
using namespace std;


namespace libconfig
{
		class Config;
		class Setting;
}

namespace core {

	/*Holds an config element, including an datatype.*/
	class ConfigEntry
	{
	public:
		enum DataType { NONE, INT, STRING, FLOAT, BOOL };
		//Load it from File; throws EmptyEntryException
		void load(libconfig::Setting *layer, libconfig::Setting *layer_write, const char* entry, int defaultVal);
		void load(libconfig::Setting *layer, libconfig::Setting *layer_write, const char* entry, float defaultVal);
		void load(libconfig::Setting *layer, libconfig::Setting *layer_write, const char* entry, string defaultVal);
		void load(libconfig::Setting *layer, libconfig::Setting *layer_write, const char* entry, bool defaultVal);
		//Create default
		ConfigEntry();
		~ConfigEntry();

		//throws: EmptyEntryException, IncompatibleTypeException
		int getInt() const;
		//throws: EmptyEntryException, IncompatibleTypeException
		float getFloat() const;
		//throws: EmptyEntryException, IncompatibleTypeException
		string getString() const;
		//throws: EmptyEntryException, IncompatibleTypeException
		bool getBool() const;

		//throws: EmptyEntryException, IncompatibleTypeException
		void setInt(int i);
		//throws: EmptyEntryException, IncompatibleTypeException
		void setFloat(float f);
		//throws: EmptyEntryException, IncompatibleTypeException
		void setString(string str);
		//throws: EmptyEntryException, IncompatibleTypeException
		void setBool(bool b);

	private:
		DataType datatype;
		//unviversal data pointer
		void *data;
	};


	class Configuration
	{
	private:
		//Config instances and states at loading, later null
		libconfig::Config *cfg = nullptr;
		libconfig::Config *cfg_write = nullptr;
		libconfig::Setting *layer = nullptr;
		libconfig::Setting *layer_write = nullptr;		

	protected:
		bool rewrite = false;
		//config control operations
		void layerUp();
		void layerDown(const char * newLayer);
		void loadFile(string file);
		void writeFile(string file);
		void cleanup();

		//Translate call zu ConfigEntry load
		inline void loadE(ConfigEntry &c, const char* entry, int defaultVal) { rewrite ? c.load(nullptr, layer_write, entry, defaultVal) : c.load(layer, layer_write, entry, defaultVal); }
		inline void loadE(ConfigEntry &c, const char* entry, float defaultVal) { rewrite ? c.load(nullptr, layer_write, entry, defaultVal) : c.load(layer, layer_write, entry, defaultVal); }
		inline void loadE(ConfigEntry &c, const char* entry, string defaultVal) { rewrite ? c.load(nullptr, layer_write, entry, defaultVal) : c.load(layer, layer_write, entry, defaultVal); }
		inline void loadE(ConfigEntry &c, const char* entry, bool defaultVal) { rewrite ? c.load(nullptr, layer_write, entry, defaultVal) : c.load(layer, layer_write, entry, defaultVal); }

	public:
		Configuration();
		~Configuration();

		virtual void load(bool rewrite = false) = 0;

	};
}
