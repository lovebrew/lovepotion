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
#include "graphics.h"
#include "love.h"

#include <stdio.h>
#include <switch.h>

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

lua_State * L;
bool ERROR = false;
bool LOVE_QUIT = false;

int main()
{
	Console::Initialize();

	Graphics::Initialize();

	luaL_requiref(L, "love", Love::Initialize, 1);

	while(appletMainLoop())
	{
		if (ERROR || LOVE_QUIT)
			break;

		hidScanInput();
	
		if (hidKeysDown(CONTROLLER_P1_AUTO) & KEY_PLUS) 
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	Love::Exit(L);
}