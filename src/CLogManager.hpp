#pragma once

#include <samplog/Logger.h>
#include "CSingleton.hpp"
#include "sdk.hpp"

#include <string>
#include <map>

using samplog::LogLevel;

using Logger_t = class CSampLogger;
using LoggerId_t = unsigned int;


class CSampLogger
{
public:
	CSampLogger(std::string &&name, LogLevel level, bool debuginfo) :
		m_Module(std::move(name)),
		m_LogLevel(level),
		m_DebugInfos(debuginfo)
	{ }
	~CSampLogger() = default;

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


class CLogManager : public CSingleton<CLogManager>
{
	friend class CSingleton<CLogManager>;
private:
	CLogManager() = default;
	~CLogManager() = default;

private:
	std::map<LoggerId_t, Logger_t> m_Logs;

public:
	LoggerId_t Create(std::string logname, LogLevel level, bool debuginfo);
	inline bool Destroy(LoggerId_t logid)
	{
		return m_Logs.erase(logid) == 1;
	}

	inline bool IsValid(LoggerId_t logid)
	{
		return m_Logs.find(logid) != m_Logs.end();
	}
	inline Logger_t &GetLogger(LoggerId_t logid)
	{
		return m_Logs.at(logid);
	}
};
