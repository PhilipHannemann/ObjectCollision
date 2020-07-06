#pragma once
#include <string>

namespace core {

	class FileManager
	{
	private:
		std::string root_path;

	public:
		FileManager(std::string root);

		bool isDirExist(std::string dir);
		bool isFileExist(std::string file);
		bool canRead(std::string file);
		bool canWrite(std::string file);
		bool makeDir(std::string dir);
		~FileManager();

		std::string getFilePath(std::string file);
		std::string getDirPath(std::string file);
	};

}
