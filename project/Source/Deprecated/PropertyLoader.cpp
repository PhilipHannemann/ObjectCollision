#include "stdafx.h"

#include "PropertyLoader.h"
#include "ResourceManager.h"
#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

PropertyLoader::PropertyLoader(string filename)
{
	instream.open(RM_FILE_BASE->getFilePath(filename));
	if (!instream.is_open())
		throw runtime_error("Can't create filestream for: " + RM_FILE_BASE->getFilePath(filename));
}

string trim(string str) 
{
	if (str == "")
		return str;

	size_t pos1 = str.find_first_not_of(" ");
	size_t pos2 = str.find_last_not_of(" ");
	return str.substr(pos1, pos2 - pos1 + 1);
}

PropertyLoader::PropertyEntry* PropertyLoader::getNextEntry()
{
	std::string line;

	while (std::getline(instream, line))
	{
		string k, v;
		size_t sep = line.find('=');

		if (sep == -1)
			continue;

		k = trim(line.substr(0, sep));
		v = trim(line.substr(sep + 1));

		return new PropertyEntry(k,v);
	}

	return nullptr;
}

PropertyLoader::~PropertyLoader()
{
	instream.close();
}

PropertyLoader::PropertyEntry::PropertyEntry(string k, string v)
{
	key = k;
	value = v;
}
