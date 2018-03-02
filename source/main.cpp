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
#include <map>
#include <vector>

#include <switch.h>

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"

#include "modules/love.h"

#include "boot_lua.h"

#include "util.h"

bool ERROR = false;
bool LOVE_QUIT = false;

int main()
{
	//Console::Initialize();

	Audio::Initialize();

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

		if (luaL_dostring(L, "if love.timer then love.timer.step() end"))
			Console::ThrowError(L);

		if (luaL_dostring(L, "if love.update then love.update(love.timer.getDelta()) end"))
			Console::ThrowError(L);

		Love::Scan(L);

		gfxFlushBuffers();
		gfxSwapBuffers();
		gfxWaitForVsync();

		Timer::Tick();
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
