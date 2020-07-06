#include "stdafx.h"
#include "Logger.h"
#include "ResourceManager.h"
#include <SDL/SDL_thread.h>
#include "FileManager.h"
#include <ctime>
#include <chrono>
#include <thread>

using namespace util;

namespace core {

	ConcurrentQueue<string> *Logger::cqueue;

	int Logger::minLevel;

	bool Logger::isRunning;

	SDL_Thread *Logger::writerThread;

	void Logger::init(int logLevel)
	{
		cqueue = new ConcurrentQueue<string>();
		minLevel = logLevel;
		isRunning = true;
		writerThread = SDL_CreateThread(writerThreadFunc, "LogWriterThread", nullptr);
	}



	int Logger::writerThreadFunc(void *ptr)
	{
		ofstream writer;
		writer.open(RM_FILE_BASE->getFilePath(_LogFilePath_), ios::out | ios::trunc);
		writer << "Quantum - Log\n";

		string str;
		while (isRunning)
		{
			cqueue->wait_and_pop(str);

			if (str != "")			//fehlerhaftes pop oder abbruch
			{
				//Daten schreiben
				writer << str;
				writer.flush();
			}
		}

		/*Write everyting until buffer is empty*/

		while (true)
		{
			cqueue->force_pop(str);

			if (str != "")			//fehlerhaftes pop oder abbruch
			{
				//Daten schreiben
				writer << str;
				writer.flush();
			}
			else
			{
				break;
			}
		}

		writer.close();

		return 0;
	}

	void Logger::log(string message)
	{
		//get and format current Time
		time_t rawtime;
		struct tm timeinfo;
		char buffer[60];
		time(&rawtime);
		errno_t result = localtime_s(&timeinfo, &rawtime);
		strftime(buffer, 60, "[%d-%m-%Y %I:%M:%S] ", &timeinfo);
		std::string str(buffer);

		//add to Queue
		cqueue->push(str + message + "\n");
	}

	void Logger::debug(string message)
	{
		if (minLevel <= 0)
			log(string("[DEBUG] ") + message);
	}

	void Logger::info(string message)
	{
		if (minLevel <= 1)
			log(string("[INFO] ") + message);
	}

	void Logger::warning(string message)
	{
		if (minLevel <= 2)
			log(string("[WARNING] ") + message);
	}

	void Logger::error(string message)
	{
		if (minLevel <= 3)
			log(string("[ERROR] ") + message);
	}

	void Logger::stop()
	{
		//beendebedingung
		isRunning = false;
		//Queue abbrechen
		cqueue->abort();
		//auf Thread warten
		int threadReturnValue;
		SDL_WaitThread(writerThread, &threadReturnValue);

		delete cqueue;
	}

	void Logger::setLogLevel(int level)
	{
		minLevel = level;
		log("Log-Level changed to " + to_string(level));
	}

}