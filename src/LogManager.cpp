#include "LogManager.hpp"
#include <samplog/DebugInfo.h>

#include <string>


bool Logger::Log(LogLevel level, const char *msg, AMX *amx)
{
	bool ret_val = false;
	std::vector<samplog::AmxFuncCallInfo> call_info;
	if (m_DebugInfos && samplog::GetAmxFunctionCallTrace(amx, call_info))
		ret_val = samplog::LogMessage(m_Module.c_str(), level, msg, call_info.data(), call_info.size());
	else
		ret_val = samplog::LogMessage(m_Module.c_str(), level, msg);

	return ret_val;
}


Logger::Id LogManager::Create(std::string logname, LogLevel level, bool debuginfo)
{
	if (logname.empty())
		return 0;

	if (logname.find("plugins/") == 0)
		return 0;

	if (logname.find("log-core") == 0)
		return 0;

	Logger::Id id = 1;
	while (m_Logs.find(id) != m_Logs.end())
		++id;

	m_Logs.emplace(id, Logger(std::move(logname), level, debuginfo));
	return id;
}
