#include "DllControl.h"
#include "Logger.h"
using std::string;
using std::cout;
using std::endl;

namespace Pano{
Logger::Logger(void)
{
	this->logEnable = true;
	this->logType = LT_CONSOLE;
}


Logger::~Logger(void)
{
}

bool Logger::getLogEnable() const
{
	return this->logEnable;
}

void Logger::setLogEnable( const bool logEnable )
{
	this->logEnable = logEnable;
}

Pano::LoggerType Logger::getLogType() const
{
	return this->logType;
}

void Logger::setLogType( const LoggerType logType )
{
	this->logType = logType;
}

void Logger::info( const std::string info, int line, char * file)
{
	if (this->logEnable)
	{
		if (this->logType == LT_CONSOLE)
		{
				cout << "[Info]: " << info << "  [FILE=" << file << "," << "LINE=" << line << "]" << endl;
		}
	}
}

void Logger::error(const std::string error, int line, char *file)
{
	if (this->logEnable)
	{
		if (this->logType == LT_CONSOLE)
		{
			cout << "[Error]: " << error << "  [FILE=" << file << "," << "LINE=" << line << "]" << endl;
		}
	}
}

Logger & Pano::Logger::instance()
{
	static Logger logger;
	return logger;
}
}