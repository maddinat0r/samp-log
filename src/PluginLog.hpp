#pragma once

#include <samplog/samplog.hpp>
#include "Singleton.hpp"

#include <fmt/format.h>

using samplog::PluginLogger_t;
using samplog::LogLevel;
using samplog::AmxFuncCallInfo;


class DebugInfoManager : public Singleton<DebugInfoManager>
{
	friend class Singleton<DebugInfoManager>;
	friend class ScopedDebugInfo;
private:
	DebugInfoManager() = default;
	~DebugInfoManager() = default;

private:
	bool m_Available = false;

	AMX *m_Amx = nullptr;
	std::vector<AmxFuncCallInfo> m_Info;
	const char *m_NativeName = nullptr;

private:
	void Update(AMX * const amx, const char *func);
	void Clear();

public:
	inline AMX * GetCurrentAmx()
	{
		return m_Amx;
	}
	inline decltype(m_Info) const &GetCurrentInfo()
	{
		return m_Info;
	}
	inline bool IsInfoAvailable()
	{
		return m_Available;
	}
	inline const char *GetCurrentNativeName()
	{
		return m_NativeName;
	}
};


class PluginLog : public Singleton<PluginLog>
{
	friend class Singleton<PluginLog>;
	friend class ScopedDebugInfo;
private:
	PluginLog() :
		m_Logger("log-plugin")
	{ }
	~PluginLog() = default;

public:
	inline bool IsLogLevel(LogLevel level)
	{
		return m_Logger.IsLogLevel(level);
	}

	template<typename... Args>
	inline void Log(LogLevel level, const char *msg)
	{
		m_Logger.Log(level, msg);
	}

	template<typename... Args>
	inline void Log(LogLevel level, const char *format, Args &&...args)
	{
		auto str = fmt::format(format, std::forward<Args>(args)...);
		m_Logger.Log(level, str.c_str());
	}

	template<typename... Args>
	inline void Log(LogLevel level, std::vector<AmxFuncCallInfo> const &callinfo,
		const char *msg)
	{
		m_Logger.Log(level, msg, callinfo);
	}

	template<typename... Args>
	inline void Log(LogLevel level, std::vector<AmxFuncCallInfo> const &callinfo,
		const char *format, Args &&...args)
	{
		auto str = fmt::format(format, std::forward<Args>(args)...);
		m_Logger.Log(level, str.c_str(), callinfo);
	}

	// should only be called in native functions
	template<typename... Args>
	void LogNative(LogLevel level, const char *fmt, Args &&...args)
	{
		if (!IsLogLevel(level))
			return;

		if (DebugInfoManager::Get()->GetCurrentAmx() == nullptr)
			return; //do nothing, since we're not called from within a native func

		std::string msg = fmt::format("{:s}: {:s}",
			DebugInfoManager::Get()->GetCurrentNativeName(),
			fmt::format(fmt, std::forward<Args>(args)...));

		if (DebugInfoManager::Get()->IsInfoAvailable())
			Log(level, DebugInfoManager::Get()->GetCurrentInfo(), msg.c_str());
		else
			Log(level, msg.c_str());
	}

private:
	PluginLogger_t m_Logger;

};


class ScopedDebugInfo
{
public:
	ScopedDebugInfo(AMX * const amx, const char *func, 
		cell * const params, const char *params_format = "");
	~ScopedDebugInfo()
	{
		DebugInfoManager::Get()->Clear();
	}
	ScopedDebugInfo(const ScopedDebugInfo &rhs) = delete;
};
