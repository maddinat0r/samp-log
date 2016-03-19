#include "CLogManager.hpp"

#include <cstring>


LoggerId_t CLogManager::Create(const char *logname)
{
	if (logname == nullptr || strlen(logname) == 0)
		return 0;

	LoggerId_t id = 1;
	while (m_Logs.find(id) != m_Logs.end())
		++id;

	m_Logs.insert({ id, samplog::CreateLogger(logname) });
	return id;
}
