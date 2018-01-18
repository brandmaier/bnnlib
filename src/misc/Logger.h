#ifndef LOGGER_H_
#define LOGGER_H_

#include <string.h>
#include <sstream>
#include <stdio.h>

struct Logger
{
	static unsigned int target;
	static const unsigned int CONSOLE = 0;
	static const unsigned int FILE = 1;
	
	static const int WARNING = 0;
	static const int ERROR = 1;
	static const int DEBUG = 2;
	static const int MESSAGE = 3;

	//Logger() = 0;
	
	static void log(char* message, int type);
	
};

#endif /*LOGGER_H_*/
