#include "stdafx.h"

#define _SCL_SECURE_NO_WARNINGS
#include "PackageProvider.h"
#include <iostream>
#include <sstream>
#include "Compression.h"
#include <algorithm>

using namespace std;

#define READ(data) fs.read((char*)&data, sizeof(data))
#define WRITE(data) fs.write((char*)&data, sizeof(data))

//standart load
PackageProvider::PackageProvider(std::string file)
{
	fs.open(file.c_str(), std::fstream::in | std::fstream::binary);
	loadFile();
}

//load file entrys and add it to filemap
void PackageProvider::loadFile()
{

	int ecount;
	READ(ecount);

	int headlen;
	READ(headlen);

	int incoff = 0;

	for (int i = 0; i < ecount; i++)
	{
		int length;
		READ(length);

		char nlen;
		READ(nlen);

		char *name = new char[nlen + 1];

		fs.read(name, nlen);
		name[nlen] = '\0';

		MapEntry me;
		me.length = length;
		me.offset = headlen + incoff;
		incoff += length;

		filemap.insert(pair<string, MapEntry>(string(name), me));
	}
	
}

PackageProvider::~PackageProvider()
{
	fs.close();
}

std::istream *PackageProvider::readFile(std::string rname)
{
	std::stringstream *buffer = new std::stringstream(std::ios_base::out | std::ios_base::binary | std::ios_base::in);

	fs.seekg(filemap[rname].offset);

	util::Compression::decompress(fs, *buffer);

	return buffer;
}


#ifdef _QPAK_CREATOR_
//creator mode constructor
PackageProvider::PackageProvider(std::string file, bool neu)
{
	if (!neu)
	{
		throw runtime_error("not supported");
	}
	else
	{
		fs.open(file.c_str(), std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);
	}

}

//pack the content from clist relative to dir in current archive
void PackageProvider::packFolder(const char * clist, std::string dir)
{
	fs.seekp(0);

	std::ifstream cls(clist, std::ios_base::in);

	if (!cls) 
	{
		cout << "Can't open ContentList!" << endl;
		return;
	}

	cout << "Reading Content-List..." << endl;

	while (cls)
	{
		char name[256];

		cls.getline(name, 256);

		string str = string(name);

		if (str == "")
			break;

		MapEntry me;
		me.sfile = (dir + str);

		std::replace(str.begin(), str.end(), '/', '.');

		filemap.insert(pair<string, MapEntry>(string(name), me));
	}

	cls.close();


	cout << "Writing File-Index..." << endl;

	int count = filemap.size();
	WRITE(count);

	int placeholder = 0;
	WRITE(placeholder);

	int hlen = 8;

	//write index
	for (std::map<string, MapEntry>::iterator it = filemap.begin(); it != filemap.end(); ++it)
	{
		string name = (*it).first;
		MapEntry *me = &(*it).second;

		me->entrypos = fs.tellp();

		WRITE(placeholder);
		char slen = name.size();
		WRITE(slen);
		fs.write(name.c_str(), slen);

		hlen += slen + sizeof(char) + sizeof(int);
	}

	cout << "Compressing files:" << endl;

	//write files -- compress and append to qpak
	int i = 0;
	for (std::map<string, MapEntry>::iterator it = filemap.begin(); it != filemap.end(); ++it)
	{
		MapEntry *me = &(*it).second;

		i++;
		cout << "Processing '" << (*it).first << "'... (" << i << "/" << count << ")" << endl;

		std::ifstream ins(me->sfile, std::ios_base::in | std::ios_base::binary);

		me->length = Compression::compress(ins, fs);
	}

	cout << "Updating index..." << endl;

	//update header length
	fs.seekp(4);
	WRITE(hlen);

	//update contentlist -- write content length to index
	for (std::map<string, MapEntry>::iterator it = filemap.begin(); it != filemap.end(); ++it)
	{
		MapEntry *me = &(*it).second;

		fs.seekp(me->entrypos);

		WRITE(me->length);
	}
}
#endif