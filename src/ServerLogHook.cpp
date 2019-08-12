#include "ServerLogHook.hpp"

#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <cctype>


void ServerLogHook::logprintf(char* format, ...)
{
	va_list args;
	va_start(args, format);

	char buffer[512];
	vsprintf(buffer, format, args);

	va_end(args);


	auto msg = std::string(buffer);
	msg.erase(
		std::remove_if(
			msg.begin(),
			msg.end(),
			[](unsigned char c) { return std::iscntrl(c); }));

	samplog::LogLevel level;
	if (msg.find(": Run time error ") != std::string::npos)
		level = samplog::LogLevel::FATAL;
	else
		level = samplog::LogLevel::INFO;

	ServerLogHook::Get()->_log->Log(level, std::move(msg));
}

void ServerLogHook::Install(void *logprintf_func)
{
	_log = samplog::Api::Get()->CreateLogger("samp-server");
	_hook.Install(logprintf_func, reinterpret_cast<void *>(ServerLogHook::logprintf));
}
