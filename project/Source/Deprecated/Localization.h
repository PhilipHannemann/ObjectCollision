#pragma once
#include <string>
#include <unordered_map>

class Localization
{
private:
	std::unordered_map<std::string, std::string> *data;

public:
	Localization();
	~Localization();
	//Cleanup and Load new language
	void init(std::string langname);
	//get match for entry; returns entry if failed
	std::string get(std::string entry);


};

