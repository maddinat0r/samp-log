#include "CLogManager.hpp"
#include <samplog/DebugInfo.h>

#include <string>


bool CSampLogger::Log(LogLevel level, const char *msg, AMX *amx)
{
	int line = 0;
	const char
		*file = "",
		*func = "";

	if (m_DebugInfos && samplog::GetLastAmxLine(amx, line))
	{
		samplog::GetLastAmxFile(amx, file);
		samplog::GetLastAmxFunction(amx, func);
	}

	return samplog::LogMessage(m_Module.c_str(), level, msg, line, file, func);
}


LoggerId_t CLogManager::Create(std::string logname, LogLevel level, bool debuginfo)
{
	if (logname.empty())
		return 0;

	if (logname.find("plugins/") == 0)
		return 0;

	if (logname.find("log-core") == 0)
		return 0;

	LoggerId_t id = 1;
	while (m_Logs.find(id) != m_Logs.end())
		++id;

	m_Logs.emplace(id, CSampLogger(std::move(logname), level, debuginfo));
	return id;
}
