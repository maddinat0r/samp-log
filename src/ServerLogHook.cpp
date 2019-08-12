#include "ServerLogHook.hpp"

#include <cstdarg>
#include <cstdio>


void ServerLogHook::logprintf(char* format, ...)
{
	va_list args;
	va_start(args, format);

	char buffer[1024];
	vsprintf(buffer, format, args);

	va_end(args);

	ServerLogHook::Get()->_log->Log(samplog::LogLevel::INFO, std::string(buffer));
}

void ServerLogHook::Install(void *logprintf_func)
{
	_log = samplog::Api::Get()->CreateLogger("samp-server");
	_hook.Install(logprintf_func, ServerLogHook::logprintf);
}
