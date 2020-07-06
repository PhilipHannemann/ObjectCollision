#pragma once
#include "ConcurrentQueue.h"
#include <string>
#include <iostream>
#include <fstream>

#define _LogFilePath_ "latest.log"

namespace core {

	class Logger
	{
		static util::ConcurrentQueue<std::string> *cqueue;

		static int writerThreadFunc(void *ptr);

		static int minLevel;

		static bool isRunning;

		static SDL_Thread *writerThread;

	public:
		static void log(std::string message);

		static void init(int logLevel);

		static void debug(std::string message);

		static void info(std::string message);

		static void warning(std::string message);

		static void error(std::string message);

		static void stop();

		static void setLogLevel(int level);

	};

}

