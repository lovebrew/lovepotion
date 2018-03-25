#include "common/runtime.h"
#include "common/version.h"

bool CONSOLE_INITIALIZED = false;

void Console::Initialize(lua_State * L, bool error)
{
	//gfxInitDefault();

	if (!CONSOLE_INITIALIZED)
		consoleInit(NULL);

	CONSOLE_INITIALIZED = true;
}

bool Console::IsInitialized()
{
	return CONSOLE_INITIALIZED;
}