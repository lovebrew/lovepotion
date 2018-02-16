extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <string>

#include "console.h"
#include "filesystem.h"
#include "graphics.h"
#include "love.h"
#include "timer.h"

#include <stdio.h>
#include <stdarg.h>

#include <switch.h>

#include "boot_lua.h"
#include "buffer_lua.h"
#include "wrap_gamepad.h"

#include "util.h"

lua_State * L;
bool ERROR = false;
bool LOVE_QUIT = false;

int main()
{
	Console::Initialize();

	//Graphics::Initialize();

	//Filesystem::Initialize();


	L = luaL_newstate();
	
	luaL_dostring(L, "jit.off()");

	luaL_openlibs(L);

	luaL_requiref(L, "love", Love::Initialize, 1);

	if (luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot"))
		Console::ThrowError(L);

	if (luaL_dobuffer(L, (char *)buffer_lua, buffer_lua_size, "buffer"))
		Console::ThrowError(L);

	gamepadNew(L);

	while(appletMainLoop())
	{
		Love::Scan(L);

		if (ERROR || LOVE_QUIT)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	while (ERROR)
		printf("Rip\n");

	Love::Exit(L);

	return 0;
}