#include "natives.hpp"
#include "version.hpp"
#include "sdk.hpp"
#include "CLogManager.hpp"

#include <samplog/DebugInfo.h>


extern void	*pAMXFunctions;
logprintf_t logprintf;


PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES; 
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	
	logprintf(" >> plugin.log: v" LOG_PLUGIN_VERSION " successfully loaded.");
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() 
{
	CLogManager::CSingleton::Destroy();
	logprintf("plugin.log: Plugin unloaded."); 
}


extern "C" const AMX_NATIVE_INFO native_list[] = 
{
	AMX_DEFINE_NATIVE(CreateLog)
	AMX_DEFINE_NATIVE(DestroyLog)
	AMX_DEFINE_NATIVE(SetLogLevel)
	AMX_DEFINE_NATIVE(IsLogLevel)
	AMX_DEFINE_NATIVE(Log)
	{NULL, NULL}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	samplog::RegisterAmx(amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	samplog::EraseAmx(amx);
	return AMX_ERR_NONE;
}
