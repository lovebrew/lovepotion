#include "common/runtime.h"

#include "modules/graphics.h"
#include "common/version.h"

#include <stdarg.h>

bool CONSOLE_INITIALIZED = false;
string CONSOLE_ERROR = "";

void Console::Initialize(bool error)
{
	if (!Graphics::IsInitialized())
		gfxInitDefault();

	consoleInit(NULL);

	if (error)
		printf(CONSOLE_ESC(1;31m));
	
	CONSOLE_INITIALIZED = true;
}

void Console::ThrowError(const string & format, ...)
{
	va_list args;

	va_start(args, format);

	vsprintf((char *)CONSOLE_ERROR.data(), format.c_str(), args);

	va_end(args);
	
	ERROR = true;
}

void Console::ThrowError(const string & errorMessage)
{
	printf("%s\n", errorMessage.c_str());

	ERROR = true;
}

const char * Console::GetError()
{
	return CONSOLE_ERROR.c_str();
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