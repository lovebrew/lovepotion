extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <3ds.h>
#include <citro2d.h>

#include "love.h"
#include "boot_lua.h"

#include <string>
#include <vector>

#include <stdio.h>

#include "audio.h"
#include "console.h"
#include "graphics.h"
#include "filesystem.h"
#include "keyboard.h"
#include "system.h"
#include "timer.h"

#include "wrap_source.h"

volatile bool updateAudioThread = true;

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

lua_State * L;
bool ERROR = false;
bool LOVE_QUIT = false;

int main()
{
	Graphics::Initialize();

	Audio::Initialize();

	System::Initialize();

	Keyboard::Initialize();

	Filesystem::Initialize();

	L = luaL_newstate();

	luaL_openlibs(L);

	luaL_requiref(L, "love", Love::Initialize, 1);

	if (luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot"))
		Console::ThrowError(L);

	while (aptMainLoop())
	{
		if (ERROR || LOVE_QUIT)
			break;

		Keyboard::Scan(L);

		if (luaL_dostring(L, "if love.timer then love.timer.step() end"))
			Console::ThrowError(L);

		if (luaL_dostring(L, "if love.update then love.update(love.timer.getDelta()) end"))
			Console::ThrowError(L);

		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		Graphics::Clear(GFX_TOP, GFX_LEFT);
		Graphics::DrawOn(GFX_TOP, GFX_LEFT);

		if (luaL_dostring(L, "if love.draw then love.draw() end"))
			Console::ThrowError(L);

		if (gfxIs3D())
		{
			Graphics::Clear(GFX_TOP, GFX_RIGHT);
			Graphics::DrawOn(GFX_TOP, GFX_RIGHT);

			if (luaL_dostring(L, "if love.draw then love.draw() end"))
				Console::ThrowError(L);
		}

		if (Console::GetScreen() != GFX_BOTTOM)
		{
			Graphics::Clear(GFX_BOTTOM, GFX_LEFT);
			Graphics::DrawOn(GFX_BOTTOM, GFX_LEFT);

			if (luaL_dostring(L, "if love.draw then love.draw() end"))
				Console::ThrowError(L);
		}

		Timer::Tick();

		C3D_FrameEnd(0);
	}

	if (ERROR)
	{
		const char * error = Console::GetError();

		if (error != NULL)
		{
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

			Graphics::Clear(GFX_TOP, GFX_LEFT);
			Graphics::DrawOn(GFX_TOP, GFX_LEFT);

			lua_getfield(L, LUA_GLOBALSINDEX, "love");
			lua_getfield(L, -1, "errhand");
			lua_remove(L, -2);

			lua_pushstring(L, error);
			lua_call(L, 1, 0);
		}
	}
	
	Love::Exit(L);

	return 0;
}