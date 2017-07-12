#include "natives.hpp"
#include "LogManager.hpp"
#include "PluginLog.hpp"

#include <fmt/format.h>


// native SetLogPluginLogLevel(E_LOGLEVEL:level);
AMX_DECLARE_NATIVE(Native::SetLogPluginLogLevel)
{
	ScopedDebugInfo dbg_info(amx, "SetLogPluginLogLevel", params, "d");

	PluginLog::Get()->SetLogLevel(static_cast<samplog::LogLevel>(params[1]));
	return 1;
}

// native Logger:CreateLog(const name[], E_LOGLEVEL:level = INFO | WARNING | ERROR, bool:debuginfo = true);
AMX_DECLARE_NATIVE(Native::CreateLog)
{
	ScopedDebugInfo dbg_info(amx, "CreateLog", params, "sdd");

	const char *name = nullptr;
	amx_StrParam(amx, params[1], name);

	cell ret_val = LogManager::Get()->Create(
		name, static_cast<LogLevel>(params[2]), params[3] != 0);
	PluginLog::Get()->LogNative(LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native DestroyLog(Logger:logger);
AMX_DECLARE_NATIVE(Native::DestroyLog)
{
	ScopedDebugInfo dbg_info(amx, "DestroyLog", params, "d");

	const Logger::Id logid = params[1];
	if (LogManager::Get()->IsValid(logid) == false)
	{
		PluginLog::Get()->LogNative(LogLevel::ERROR, "invalid log id '{}'", logid);
		return 0;
	}

	cell ret_val = LogManager::Get()->Destroy(logid) ? 1 : 0;
	PluginLog::Get()->LogNative(LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native SetLogLevel(Logger:logger, E_LOGLEVEL:level);
AMX_DECLARE_NATIVE(Native::SetLogLevel)
{
	ScopedDebugInfo dbg_info(amx, "SetLogLevel", params, "dd");

	const Logger::Id logid = params[1];
	if (LogManager::Get()->IsValid(logid) == false)
	{
		PluginLog::Get()->LogNative(LogLevel::ERROR, "invalid log id '{}'", logid);
		return 0;
	}

	LogManager::Get()->GetLogger(logid).
		LogLevel = static_cast<samplog::LogLevel>(params[2]);

	return 1;
}

// native bool:IsLogLevel(Logger:logger, E_LOGLEVEL:level);
AMX_DECLARE_NATIVE(Native::IsLogLevel)
{
	ScopedDebugInfo dbg_info(amx, "IsLogLevel", params, "dd");

	const Logger::Id logid = params[1];
	if (LogManager::Get()->IsValid(logid) == false)
	{
		PluginLog::Get()->LogNative(LogLevel::ERROR, "invalid log id '{}'", logid);
		return 0;
	}

	cell ret_val = LogManager::Get()->GetLogger(logid).
		IsLogLevel(static_cast<samplog::LogLevel>(params[2])) ? 1 : 0;
	PluginLog::Get()->LogNative(LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}

// native Log(Logger:logger, E_LOGLEVEL:level, const msg[], {Float,_}:...);
AMX_DECLARE_NATIVE(Native::Log)
{
	ScopedDebugInfo dbg_info(amx, "Log", params, "dds");

	const Logger::Id logid = params[1];
	if (LogManager::Get()->IsValid(logid) == false)
	{
		PluginLog::Get()->LogNative(LogLevel::ERROR, "invalid log id '{}'", logid);
		return 0;
	}

	auto &logger = LogManager::Get()->GetLogger(logid);

	const samplog::LogLevel loglevel = static_cast<decltype(loglevel)>(params[2]);
	if (!logger.IsLogLevel(loglevel))
	{
		PluginLog::Get()->LogNative(LogLevel::DEBUG, 
			"log level not set, not logging message (current log level: '{}')",
			logger.LogLevel);
		return 0;
	}


	std::string format_str = amx_GetCppString(amx, params[3]);

	const unsigned int
		first_param_idx = 4,
		num_args = (params[0] / sizeof(cell)),
		num_dyn_args = num_args - (first_param_idx - 1);
	unsigned int param_counter = 0;

	fmt::MemoryWriter str_writer;

	size_t
		spec_pos = std::string::npos,
		spec_offset = 0;

	while ((spec_pos = format_str.find('%', spec_offset)) != std::string::npos)
	{
		if (param_counter >= num_dyn_args)
		{
			PluginLog::Get()->LogNative(LogLevel::ERROR,
				"more format specifiers found than arguments passed");
			return 0;
		}

		if (spec_pos == (format_str.length() - 1))
		{
			PluginLog::Get()->LogNative(LogLevel::ERROR,
				"invalid specifier at string end");
			return 0;
		}

		str_writer << format_str.substr(spec_offset, spec_pos - spec_offset);
		spec_offset = spec_pos + 2; // 2 = '%' + char specifier (like 'd' or 's')

		cell *param_addr = nullptr;

		const char formt_spec = format_str.at(spec_pos + 1);
		switch (formt_spec)
		{
		case '%': // '%' escape
			str_writer << '%';
			break;
		case 's': // string
			str_writer << amx_GetCppString(amx, params[first_param_idx + param_counter]);
			break;
		case 'd': // decimal
		case 'i': // integer
			amx_GetAddr(amx, params[first_param_idx + param_counter], &param_addr);
			str_writer << static_cast<int>(*param_addr);
			break;
		case 'f': // float
			amx_GetAddr(amx, params[first_param_idx + param_counter], &param_addr);
			str_writer << amx_ctof(*param_addr);
			break;
		default:
			PluginLog::Get()->LogNative(LogLevel::ERROR,
				"invalid format specifier '%{}'", formt_spec);
			return 0;
		}

		param_counter++;
	}

	// copy rest of format string
	str_writer << format_str.substr(spec_offset);
	
	cell ret_val = logger.Log(loglevel, str_writer.c_str(), amx) ? 1 : 0;
	PluginLog::Get()->LogNative(LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}
