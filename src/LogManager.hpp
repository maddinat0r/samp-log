#pragma once

#include <samplog/Logger.h>
#include "CSingleton.hpp"
#include "sdk.hpp"

#include <string>
#include <map>


class Logger
{
public:
	using Id = unsigned int;

public:
	Logger(std::string &&name, samplog::LogLevel level, bool debuginfo) :
		m_Module(std::move(name)),
		LogLevel(level),
		m_DebugInfos(debuginfo)
	{ }
	~Logger() = default;

public:
	bool Log(samplog::LogLevel level, const char *msg, AMX *amx);

	inline bool IsLogLevel(samplog::LogLevel log_level) const
	{
		return (LogLevel & log_level) == log_level;
	}

public:
	samplog::LogLevel LogLevel;

private:
	std::string m_Module;
	bool m_DebugInfos;

};


class LogManager : public CSingleton<LogManager>
{
	friend class CSingleton<LogManager>;
private:
	LogManager() = default;
	~LogManager() = default;

private:
	std::map<Logger::Id, Logger> m_Logs;

public:
	Logger::Id Create(std::string logname, samplog::LogLevel level, bool debuginfo);
	inline bool Destroy(Logger::Id logid)
	{
		return m_Logs.erase(logid) == 1;
	}

	inline bool IsValid(Logger::Id logid)
	{
		return m_Logs.find(logid) != m_Logs.end();
	}
	inline Logger &GetLogger(Logger::Id logid)
	{
		return m_Logs.at(logid);
	}
};
