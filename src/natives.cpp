#include "natives.hpp"
#include "CLogManager.hpp"

#include <cppformat/format.h>
#include <samplog/DebugInfo.hpp>

#include <set>
#include <cstring>


static samplog::LogLevel GetLogLevelFromPawn(cell levelid)
{
	switch (levelid)
	{
	case 1: // DEBUG
		return samplog::LogLevel::DEBUG;
		break;
	case 2: // INFO
		return samplog::LogLevel::INFO;
		break;
	case 4: // WARNING
		return samplog::LogLevel::WARNING;
		break;
	case 8: // ERROR
		return samplog::LogLevel::ERROR;
		break;
	default:
		return samplog::LogLevel::NONE;
	}
}

// native Logger:CreateLog(const name[], E_LOGLEVEL:level = INFO | WARNING | ERROR);
AMX_DECLARE_NATIVE(Native::CreateLog)
{
	const char *name = nullptr;
	cell pawn_loglevel = params[2];
	amx_StrParam(amx, params[1], name);

	if (strstr(name, "plugins") == name)
		return false;

	LoggerId_t logid = CLogManager::Get()->Create(name);
	auto &logger = CLogManager::Get()->GetLogger(logid);
	std::set<samplog::LogLevel> levels{
		samplog::LogLevel::DEBUG,
		samplog::LogLevel::INFO,
		samplog::LogLevel::WARNING,
		samplog::LogLevel::ERROR
	};

	for (auto &l : levels)
	{
		if (pawn_loglevel & static_cast<std::underlying_type<samplog::LogLevel>::type>(l))
			logger->SetLogLevel(l, true);
	}

	return logid;
}

// native DestroyLog(Logger:logger);
AMX_DECLARE_NATIVE(Native::DestroyLog)
{
	const LoggerId_t logid = params[1];
	if (CLogManager::Get()->IsValid(logid) == false)
		return 0;

	return CLogManager::Get()->Destroy(logid);
}

// native SetLogLevel(Logger:logger, E_LOGLEVEL:level, bool:enabled);
AMX_DECLARE_NATIVE(Native::SetLogLevel)
{
	const LoggerId_t logid = params[1];
	if (CLogManager::Get()->IsValid(logid) == false)
		return 0;

	samplog::LogLevel loglevel = GetLogLevelFromPawn(params[2]);
	if (loglevel == samplog::LogLevel::NONE)
		return 0;

	auto &logger = CLogManager::Get()->GetLogger(logid);
	logger->SetLogLevel(loglevel, params[2] != 0);
	return 1;
}

// native bool:IsLogLevel(Logger:logger, E_LOGLEVEL:level);
AMX_DECLARE_NATIVE(Native::IsLogLevel)
{
	const LoggerId_t logid = params[1];
	if (CLogManager::Get()->IsValid(logid) == false)
		return 0;

	samplog::LogLevel loglevel = GetLogLevelFromPawn(params[2]);
	if (loglevel == samplog::LogLevel::NONE)
		return 0;

	auto &logger = CLogManager::Get()->GetLogger(logid);
	return logger->IsLogLevel(loglevel);
}

// native Log(Logger:logger, E_LOGLEVEL:level, const msg[], {Float,_}:...);
AMX_DECLARE_NATIVE(Native::Log)
{
	const LoggerId_t logid = params[1];
	if (CLogManager::Get()->IsValid(logid) == false)
		return 0;

	samplog::LogLevel loglevel = GetLogLevelFromPawn(params[2]);
	if (loglevel == samplog::LogLevel::NONE)
		return 0;


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
			return false; // too many args given

		if (spec_pos == (format_str.length() - 1))
			return false;

		str_writer << format_str.substr(spec_offset, spec_pos - spec_offset);
		spec_offset = spec_pos + 2; // 2 = '%' + char specifier (like 'd' or 's')

		cell *param_addr = nullptr;

		switch (format_str.at(spec_pos + 1))
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
			return false;
		}

		param_counter++;
	}

	// copy rest of format string
	str_writer << format_str.substr(spec_offset);
	
	auto &logger = CLogManager::Get()->GetLogger(logid);
	int line = 0;
	const char
		*file = "",
		*func = "";

	if ((amx->flags & AMX_FLAG_DEBUG) == AMX_FLAG_DEBUG)
	{
		if (samplog::GetLastAmxLine(amx, line))
		{
			samplog::GetLastAmxFile(amx, file);
			samplog::GetLastAmxFunction(amx, func);
		}
	}
	logger->Log(str_writer.c_str(), loglevel, line, file, func);
	return 1;
}
