#include "runtime.h"

#include "graphics.h"
#include "version.h"

#include <stdarg.h>

#include <switch.h>

bool CONSOLE_INITIALIZED = false;
char * CONSOLE_ERROR = "";

void Console::Initialize()
{
	if (!Graphics::IsInitialized())
		gfxInitDefault();

	CONSOLE_SCREEN = screen;

	consoleInit(NULL);

	printf("\e[1;36mLOVE\e[0m %s for 3DS\n\n", Love::VERSION.c_str());

	CONSOLE_INITIALIZED = true;
}

void Console::ThrowError(const string & format, ...)
{
	va_list args;
	va_list echo;

	va_start(args, format);
	va_copy(echo, args);

	vsprintf(CONSOLE_ERROR, format.c_str(), echo);
	luaL_error(L, "%s", CONSOLE_ERROR);

	va_end(args);
	
	ERROR = true;
}

char * Console::GetError()
{
	return CONSOLE_ERROR;
}

int Console::ThrowError(lua_State * L)
{
	const char * errorMessage = nullptr;
	if (!lua_isnil(L, -1))
	{
		errorMessage = lua_tostring(L, -1);

		Console::ThrowError("%s", errorMessage);
	}

	return 0;
}

bool Console::IsInitialized()
{
	return CONSOLE_INITIALIZED;
}