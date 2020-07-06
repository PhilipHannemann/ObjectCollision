#pragma once
#include <map>
#include <istream>
#include <fstream>
#include <string>

class PackageProvider
{
	//Entry for the Filemap -- extended in Creator mode
	struct MapEntry 
	{
		int offset;
		int length;
		#ifdef _QPAK_CREATOR_
		int entrypos;
		std::string sfile;
		#endif
	};

	//filemap
	std::map<std::string, MapEntry> filemap;
	//stream to qpak
	std::fstream fs;

	void loadFile();


#ifdef _QPAK_CREATOR_
public:
	//Creator functions
	PackageProvider(std::string file, bool neu);
	void packFolder(const char * clist, std::string dir);

#endif

public:
	//Standart functions
	PackageProvider(std::string file);
	~PackageProvider();
	std::istream *readFile(std::string rname);

};

