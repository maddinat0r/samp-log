#pragma once

#include "Singleton.hpp"

#include <samplog/samplog.hpp>
#include <subhook.h>


class ServerLogHook : public Singleton<ServerLogHook>
{
	friend class Singleton<ServerLogHook>;
private:
	ServerLogHook() = default;
	~ServerLogHook() = default;

private:
	subhook::Hook _hook;
	samplog::Logger_t _log;

private:
	static void logprintf(char* format, ...);

public:
	void Install(void *logprintf_func);
};
