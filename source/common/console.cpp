#include "runtime.h"
#include "graphics.h"
#include "version.h"

#include <stdarg.h>

gfxScreen_t CONSOLE_SCREEN;
bool CONSOLE_INITIALIZED = false;
char * CONSOLE_ERROR = "";

void Console::Initialize(gfxScreen_t screen)
{
	if (!Graphics::IsInitialized())
		gfxInitDefault();

	CONSOLE_SCREEN = screen;

	consoleInit(screen, NULL);

	printf("\e[1;36mLOVE\e[0m %s for 3DS\n\n", Love::VERSION.c_str());

	CONSOLE_INITIALIZED = true;
}

void Console::ThrowError(const string & format, ...)
{
	va_list args;

	va_start(args, format);

	vsprintf(CONSOLE_ERROR, format.c_str(), args);

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

gfxScreen_t Console::GetScreen()
{
	return CONSOLE_SCREEN;
}

bool Console::IsInitialized()
{
	return CONSOLE_INITIALIZED;
}