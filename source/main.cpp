extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <string>

#include "modules/audio.h"
#include "common/console.h"
#include "modules/filesystem.h"
#include "modules/graphics.h"

#include "modules/timer.h"

#include <stdio.h>
#include <stdarg.h>

#include <switch.h>

#include <vector>
#include "gamepad.h"
#include "wrap_gamepad.h"
#include "modules/love.h"

#include "boot_lua.h"

#include "util.h"

bool ERROR = false;
bool LOVE_QUIT = false;

int main()
{
	//Console::Initialize();

	//Audio::Initialize();

	//Graphics::Initialize();

	if (!Filesystem::Initialize())
		Console::ThrowError("%s", "No Game!\n");

	lua_State * L = luaL_newstate();

	luaL_openlibs(L);

	luaL_requiref(L, "love", Love::Initialize, 1);

	if (luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot"))
		Console::ThrowError(L);

	gamepadNew(L);

	while(appletMainLoop())
	{
		if (ERROR || LOVE_QUIT)
			break;

		Love::Scan(L);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();
	}

	Love::Exit(L);

	while (ERROR)
	{
		hidScanInput();
		u64 keyDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (keyDown & KEY_PLUS)
			break;
	}

	return 0;
}
