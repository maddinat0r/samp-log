#pragma once

#include <samplog/Logger.h>
#include "CSingleton.hpp"
#include "sdk.hpp"

#include <string>
#include <map>

using samplog::LogLevel;


class Logger
{
public:
	using Id = unsigned int;

public:
	Logger(std::string &&name, LogLevel level, bool debuginfo) :
		m_Module(std::move(name)),
		m_LogLevel(level),
		m_DebugInfos(debuginfo)
	{ }
	~Logger() = default;

public:
	bool Log(LogLevel level, const char *msg, AMX *amx);

	inline void SetLogLevel(LogLevel log_level)
	{
		m_LogLevel = log_level;
	}
	inline bool IsLogLevel(LogLevel log_level) const
	{
		return (m_LogLevel & log_level) == log_level;
	}


private:
	std::string m_Module;
	LogLevel m_LogLevel;
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
	Logger::Id Create(std::string logname, LogLevel level, bool debuginfo);
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
