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

#include <vector>

#include "gamepad.h"
#include "wrap_gamepad.h"

#include "boot_lua.h"

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

lua_State * L;
bool ERROR = false;
bool LOVE_QUIT = false;

int main()
{
	Console::Initialize();

	//Graphics::Initialize();

	//Filesystem::Initialize();

	L = luaL_newstate();

	luaL_openlibs(L);

	luaL_requiref(L, "love", Love::Initialize, 1);

	luaL_dostring(L, "print('OS: ' .. love.system.getOS())");
	luaL_dostring(L, "print('Screen: ' .. love.graphics.getWidth() .. 'x' .. love.graphics.getHeight())");
	luaL_dostring(L, "print('RendererInfo:')");
	luaL_dostring(L, "print(love.graphics.getRendererInfo())");
	luaL_dostring(L, "print('Press + to Quit')");

	if (luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot"))
		Console::ThrowError(L);

	while(appletMainLoop())
	{
		if (ERROR || LOVE_QUIT)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	Love::Exit(L);

	return 0;
}