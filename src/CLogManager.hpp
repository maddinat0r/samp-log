#pragma once

#include <samplog/Logger.hpp>
#include "CSingleton.hpp"

#include <unordered_map>


using LoggerId_t = unsigned int;


class CLogManager : public CSingleton<CLogManager>
{
	friend class CSingleton<CLogManager>;
private:
	CLogManager() = default;
	~CLogManager() = default;

private:
	std::unordered_map<LoggerId_t, samplog::Logger_t> m_Logs;

public:
	LoggerId_t Create(const char *logname);
	inline bool Destroy(LoggerId_t logid)
	{
		return m_Logs.erase(logid) == 1;
	}

	inline bool IsValid(LoggerId_t logid)
	{
		return m_Logs.find(logid) != m_Logs.end();
	}
	inline samplog::Logger_t &GetLogger(LoggerId_t logid)
	{
		return m_Logs.at(logid);
	}
};
