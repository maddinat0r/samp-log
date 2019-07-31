#include "natives.hpp"
#include "LogManager.hpp"
#include "PluginLog.hpp"

#include <fmt/format.h>


// native Logger:CreateLog(const name[], bool:debuginfo = true);
AMX_DECLARE_NATIVE(Native::CreateLog)
{
	ScopedDebugInfo dbg_info(amx, "CreateLog", params, "sd");

	const char *name = nullptr;
	amx_StrParam(amx, params[1], name);

	if (name == nullptr)
	{
		PluginLog::Get()->LogNative(LogLevel::ERROR, "invalid logger name");
		return 0;
	}

	auto ret_val = static_cast<cell>(LogManager::Get()->Create(name, params[2] != 0));
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
			"log level not set, not logging message");
		return 0;
	}


	std::string format_str = amx_GetCppString(amx, params[3]);

	const unsigned int
		first_param_idx = 4,
		num_args = (params[0] / sizeof(cell)),
		num_dyn_args = num_args - (first_param_idx - 1);
	unsigned int param_counter = 0;

	fmt::memory_buffer str_writer;

	size_t
		spec_pos = std::string::npos,
		spec_offset = 0;

	while ((spec_pos = format_str.find('%', spec_offset)) != std::string::npos)
	{
		if (spec_pos == (format_str.length() - 1))
		{
			PluginLog::Get()->LogNative(LogLevel::ERROR,
				"invalid specifier at string end");
			return 0;
		}

		fmt::format_to(str_writer, "{:s}",
			format_str.substr(spec_offset, spec_pos - spec_offset));
		spec_offset = spec_pos + 2; // 2 = '%' + char specifier (like 'd' or 's')

		const char format_spec = format_str.at(spec_pos + 1);
		if (format_spec == '%')
		{
			str_writer.push_back('%');
			continue;
		}

		if (param_counter >= num_dyn_args)
		{
			PluginLog::Get()->LogNative(LogLevel::ERROR,
				"format specifier at position {} has no argument passed", spec_pos);
			return 0;
		}

		cell
			*param_addr = nullptr,
			&param = params[first_param_idx + param_counter++];

		switch (format_spec)
		{
		case 's': // string
			fmt::format_to(str_writer, "{:s}", amx_GetCppString(amx, param));
			break;
		case 'd': // decimal
		case 'i': // integer
			if (amx_GetAddr(amx, param, &param_addr) != AMX_ERR_NONE || param_addr == nullptr)
			{
				PluginLog::Get()->LogNative(LogLevel::ERROR,
					"error while retrieving AMX address");
				return 0;
			}
			fmt::format_to(str_writer, "{:d}", static_cast<int>(*param_addr));
			break;
		case 'f': // float
			if (amx_GetAddr(amx, param, &param_addr) != AMX_ERR_NONE || param_addr == nullptr)
			{
				PluginLog::Get()->LogNative(LogLevel::ERROR,
					"error while retrieving AMX address");
				return 0;
			}
			fmt::format_to(str_writer, "{:f}", amx_ctof(*param_addr));
			break;
		default:
			PluginLog::Get()->LogNative(LogLevel::ERROR,
				"invalid format specifier '%{:c}'", format_spec); // TODO test "c"
			return 0;
		}
	}

	// copy rest of format string
	fmt::format_to(str_writer, "{:s}", format_str.substr(spec_offset));
	
	cell ret_val = logger.Log(loglevel, fmt::to_string(str_writer), amx) ? 1 : 0;
	PluginLog::Get()->LogNative(LogLevel::DEBUG, "return value: '{}'", ret_val);
	return ret_val;
}
