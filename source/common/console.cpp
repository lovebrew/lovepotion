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
	this->enabled = false;
}

void Console::Enable(gfxScreen_t screen)
{
	if (!this->IsEnabled())
		this->enabled = true;

	consoleInit(screen, NULL);
	this->screen = screen;
}

gfxScreen_t Console::GetScreen()
{
	return this->screen;
}

bool Console::IsEnabled()
{
	return this->enabled;
}

void Console::ThrowError(const char * errorMessage)
{
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
	const char * errorMessage = lua_tostring(L, -1);
	
	this->ThrowError(errorMessage);
}