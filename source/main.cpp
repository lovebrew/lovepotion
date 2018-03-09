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

	Graphics::Initialize();

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

		if (luaL_dostring(L, LOVE_TIMER_STEP))
			Console::ThrowError(L);

		if (luaL_dostring(L, LOVE_UPDATE))
			Console::ThrowError(L);

		luaL_dostring(L, "love.graphics.clear()");

		if (luaL_dostring(L, LOVE_DRAW))
			Console::ThrowError(L);

		Love::Scan(L);

		luaL_dostring(L, "love.graphics.present()");

		Timer::Tick();
	}

	if (ERROR)
	{
		const char * error = Console::GetError();

		if (error != NULL)
		{
			Console::Initialize(true);

			lua_getfield(L, LUA_GLOBALSINDEX, "love");
			lua_getfield(L, -1, "errhand");
			lua_remove(L, -2);

			lua_pushstring(L, error);
			lua_call(L, 1, 0);

			gfxFlushBuffers();
			gfxSwapBuffers();
			gfxWaitForVsync();

			//return luaL_error(L, "%s", error);
		}
	}

	Love::Exit(L);

	return 0;
}
