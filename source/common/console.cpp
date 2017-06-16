#include <3ds.h>
#include <stdio.h>
#include <sys/types.h>

#include "common/runtime.h"
#include "console.h"
#include "common/version.h"

using love::Console;

Console::Console()
{
	this->hasPrinted = false;
}

void Console::Enable(gfxScreen_t screen)
{
	if (!this->IsEnabled())
		love::CONSOLE_ENABLED = true;

	consoleInit(screen, NULL);
}

bool Console::IsEnabled()
{
	return love::CONSOLE_ENABLED;
}

void Console::ThrowError(const char * errorMessage)
{
	if (!this->IsEnabled())
		return;

	if (errorMessage == nullptr)
		return;
	
	if (!this->hasPrinted)
	{
		printf("\e[31;1m%s\e[0m\n", errorMessage);
		
		LUA_ERROR = true;
		this->hasPrinted = true;
	}
}

void Console::ThrowError(lua_State * L)
{
	if (!this->IsEnabled())
		return;

	const char * errorMessage = lua_tostring(L, -1);

	if (errorMessage == nullptr)
		return;
	
	if (!this->hasPrinted)
	{
		printf("\e[31;1m%s\e[0m\n", errorMessage);

		LUA_ERROR = true;
		this->hasPrinted = true;
	}
}