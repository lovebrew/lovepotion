extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <3ds.h>

#include <citro3d.h>

#include <stdlib.h>
#include <string>
#include <vector>

#include <ivorbiscodec.h>
#include <ivorbisfile.h>

#include "common/version.h"
#include "modules/love/love.h"
#include "modules/timer/timer.h"

#include "modules/audio/source.h"
#include "modules/audio/wrap_source.h"
std::vector<love::Source *> streams;

#include "modules/graphics/crendertarget.h"
#include "modules/graphics/graphics.h"
#include "modules/socket/socket.h"

bool LUA_ERROR = false;
bool LOVE_QUIT = false;
bool FUSED = false;
lua_State * L;
volatile bool updateAudioThread = true;
bool screenEnable[1];

#include <unistd.h>

#include "boot_lua.h"

#define luaL_dobuffer(L, b, n, s) (luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#include "common/console.h"
love::Console * console;

int main(int argc, char ** argv)
{
	L = luaL_newstate();

	luaL_dostring(L, "jit.off()");

	luaL_openlibs(L);
	initLuaSocket(L);

	
	Result romfs = romfsInit();
	FUSED = (romfs) ? false : true;
	
	loveChangeDir(FUSED);

	luaL_requiref(L, "love", loveInit, 1);

	console = new love::Console();
	
	luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot");

	loveCreateSaveDirectory();

	osSetSpeedupEnable(true);
	
	s32 priority = 0;
	svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
	Thread musicThread = threadCreate(sourceStream, NULL, 0x1000, priority - 1, -2, false);

	if(luaL_dostring(L, "if love.load then love.load() end"))
		console->ThrowError(L);
	
	while (aptMainLoop())
	{
		if (LOVE_QUIT)
			break;

		if (!LUA_ERROR)
		{
			loveScan(L);

			if (luaL_dostring(L, "love.timer.step()"))
				console->ThrowError(L);

			if(luaL_dostring(L, "if love.update then love.update(love.timer.getDelta()) end"))
				console->ThrowError(L);

			if (CLOSE_KEYBOARD)
			{
				luaL_dostring(L, "love.keyboard.setTextInput(false)");
				CLOSE_KEYBOARD =  false;
			}

			love::Graphics::Instance()->Render(GFX_TOP, GFX_LEFT);

			if (luaL_dostring(L, "if love.draw then love.draw() end"))
				console->ThrowError(L);
				
			if (gfxIs3D())
			{
				love::Graphics::Instance()->Render(GFX_TOP, GFX_RIGHT);
					
				if (luaL_dostring(L, "if love.draw then love.draw() end"))
					console->ThrowError(L);
			}

			if (!console->IsEnabled())
			{
				love::Graphics::Instance()->Render(GFX_BOTTOM, GFX_LEFT);

				if (luaL_dostring(L, "if love.draw then love.draw() end"))
					console->ThrowError(L);
			}

			love::Graphics::Instance()->SwapBuffers();

			love::Timer::Instance()->Tick();
		}
		else
		{
			love::Graphics::Instance()->Render(GFX_TOP, GFX_LEFT);

			const char * error = lua_tostring(L, -1);

			lua_getfield(L, LUA_GLOBALSINDEX, "love");
			lua_getfield(L, -1, "errhand");
			lua_remove(L, -2);

			if (!lua_isnil(L, -1)) 
			{
				lua_pushstring(L, error);
				lua_call(L, 1, 0);
			}

			hidScanInput();
			u32 kTempDown = hidKeysDown();
			if (kTempDown & KEY_START)
				break;
			
			if (!console->IsEnabled())
				love::Graphics::Instance()->Render(GFX_BOTTOM, GFX_LEFT);

			love::Graphics::Instance()->SwapBuffers();
		}
	}

	updateAudioThread = false;

	loveClose(L);

	threadJoin(musicThread, U64_MAX);
	threadFree(musicThread);
	
	if (FUSED) 
		romfsExit();

	return 0;
}