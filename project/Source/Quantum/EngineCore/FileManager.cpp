#include "stdafx.h"
#include "FileManager.h"
#include <direct.h>
using namespace std;

namespace core {

	FileManager::FileManager(string root)
	{
		root_path = root;
	}

	bool FileManager::isFileExist(string file) {

		struct stat info;

		if (stat(getFilePath(file).c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IFREG)
			return true;
		else
			return false;
	}

	bool FileManager::canRead(std::string file)
	{
		struct stat info;

		if (stat(getDirPath(file).c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IREAD)
			return true;
		else
			return false;
	}

	bool FileManager::canWrite(std::string file)
	{
		struct stat info;

		if (stat(getDirPath(file).c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IWRITE)
			return true;
		else
			return false;
	}

	bool FileManager::makeDir(std::string dir)
	{
		return (_mkdir((root_path + dir).c_str()) != -1);
	}

	bool FileManager::isDirExist(string file) {

		struct stat info;

		if (stat(getDirPath(file).c_str(), &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}



	FileManager::~FileManager()
	{

	}

	std::string FileManager::getFilePath(std::string file)
	{
		return root_path + file;
	}

	std::string FileManager::getDirPath(std::string dir)
	{
		return root_path + dir + "/";
	}
}