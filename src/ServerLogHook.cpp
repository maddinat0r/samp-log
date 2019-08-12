#include "ServerLogHook.hpp"

#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <cctype>


void ServerLogHook::logprintf(char* format, ...)
{
	va_list args;
	va_start(args, format);

	char buffer[1024];
	vsprintf(buffer, format, args);

	va_end(args);


	auto msg = std::string(buffer);
	msg.erase(
		std::remove_if(
			msg.begin(),
			msg.end(),
			[](unsigned char c) { return std::iscntrl(c); }));

	ServerLogHook::Get()->_log->Log(level, std::move(msg));
}

void ServerLogHook::Install(void *logprintf_func)
{
	_log = samplog::Api::Get()->CreateLogger("samp-server");
	_hook.Install(logprintf_func, ServerLogHook::logprintf);
}
