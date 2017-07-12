#include "LogManager.hpp"
#include "PluginLog.hpp"
#include <samplog/DebugInfo.h>

#include <string>


bool Logger::Log(samplog::LogLevel level, const char *msg, AMX *amx)
{
	PluginLog::Get()->Log(LogLevel::DEBUG, "Logger::Log(level={}, msg='{}', amx={})",
		level, msg, static_cast<const void *>(amx));

	bool ret_val = false;
	std::vector<samplog::AmxFuncCallInfo> call_info;
	if (m_DebugInfos && samplog::GetAmxFunctionCallTrace(amx, call_info))
	{
		PluginLog::Get()->Log(LogLevel::DEBUG, "Logger::Log - logging with debug info");
		ret_val = samplog::LogMessage(m_Module.c_str(), level, msg, 
			call_info.data(), call_info.size());
	}
	else
	{
		PluginLog::Get()->Log(LogLevel::DEBUG, "Logger::Log - logging without debug info");
		ret_val = samplog::LogMessage(m_Module.c_str(), level, msg);
	}

	return ret_val;
}


Logger::Id LogManager::Create(std::string logname, LogLevel level, bool debuginfo)
{
	PluginLog::Get()->Log(LogLevel::DEBUG, 
		"LogManager::Create(logname='{}', level={}, debuginfo={})",
		logname, level, debuginfo);

	if (logname.empty())
	{
		PluginLog::Get()->Log(LogLevel::ERROR, 
			"LogManager::Create - empty log name");
		return 0;
	}

	if (logname.find("plugins/") == 0)
	{
		PluginLog::Get()->Log(LogLevel::ERROR, 
			"LogManager::Create - invalid log name");
		return 0;
	}

	if (logname.find("log-core") == 0)
	{
		PluginLog::Get()->Log(LogLevel::ERROR,
			"LogManager::Create - invalid log name");
		return 0;
	}

	Logger::Id id = 1;
	while (m_Logs.find(id) != m_Logs.end())
		++id;

	PluginLog::Get()->Log(LogLevel::INFO, "created logger '{}' with id {}", logname, id);

	m_Logs.emplace(id, Logger(std::move(logname), level, debuginfo));
	return id;
}
