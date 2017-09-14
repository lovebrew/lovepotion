#include <3ds.h>
#include <stdio.h>
#include <sys/types.h>

#include "common/runtime.h"
#include "console.h"
#include "common/version.h"

using love::Console;

Console::Console()
{
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

const char * Console::GetError()
{
	return this->errorMessage;
}

int Console::ThrowError(const char * errorMessage)
{
	if (errorMessage == nullptr)
		return 0;

	this->errorMessage = errorMessage;
	printf("\e[31;1m%s\e[0m\n", errorMessage);
		
	LUA_ERROR = true;

	return 0;
}

int Console::ThrowError(lua_State * L)
{
	const char * errorMessage = nullptr;
	if (!lua_isnil(L, -1))
	{
		errorMessage = lua_tostring(L, -1);
	
		this->ThrowError(errorMessage);
	}
}