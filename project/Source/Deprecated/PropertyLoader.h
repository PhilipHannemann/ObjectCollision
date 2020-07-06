#pragma once
/*
*	class PropertyLoader
*
*	Loads an propertiy file using the style:
*	key=value\n
*	
*	Important:
*	delete PropertyEntry after use
*/

#include <string>
#include <fstream>

class PropertyLoader
{
private:
	std::ifstream instream;
public:
	//class for renturn entries, containing key and value
	struct PropertyEntry 
	{
		std::string key;
		std::string value;
		PropertyEntry(std::string k, std::string v);
	};

	//open stream at first line
	PropertyLoader(std::string filename);
	//read next valid line and return pointer to the Entry; delete the Entry after use
	PropertyEntry* getNextEntry();
	//close the Stream
	~PropertyLoader();
};

