#ifndef PANO_LOGER_INCLUDE_H_
#define PANO_LOGER_INCLUDE_H_
#pragma once

#ifdef DLL_CLASS 
#else 
#define DLL_CLASS __declspec(dllimport)
#endif 
#define LOG_INFO(str) Pano::Logger::instance().info(str, __LINE__, __FILE__)
#define LOG_ERROR(str) Pano::Logger::instance().error(str, __LINE__, __FILE__)

#include <string>
#include <iostream>
#include <vector>
#include <map>

using std::vector;
using std::map;

namespace Pano{


enum LoggerType{
	LT_CONSOLE = 1,
	LT_FILE = 2,
	LT_BOTH = 3
};

class DLL_CLASS Logger
{
private:
	Logger(void);
	~Logger(void);
	Logger(const Logger &);
	Logger & operator=(const Logger &);

private:
	bool logEnable;
	LoggerType logType;

public:
	static Logger & instance();

	bool getLogEnable() const;
	void setLogEnable(const bool logEnable);

	LoggerType getLogType() const;
	void setLogType(const LoggerType logType);

	void info(const std::string, int line, char *file);
	void error(const std::string, int line, char *file);
};
}


#endif