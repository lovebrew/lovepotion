extern "C" 
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.2.h>
    #include <luaobj.h>
}

#include <string>
#include <3ds.h>

#include "common/console.h"
#include "modules/timer.h"
#include "modules/love.h"
#include "common/util.h"

#include "modules/filesystem.h"
#include "modules/system.h"

#include "boot_lua.h"

bool ERROR = false;
bool LOVE_QUIT = false;

int main(int argc, char **argv)
{
	gfxInitDefault();

	consoleInit(GFX_TOP, NULL);

	System::Initialize();

	Filesystem::Initialize();

    lua_State * L = luaL_newstate();

    luaL_openlibs(L);

	luaL_requiref(L, "love", Love::Initialize, 1);

	luaL_dobuffer(L, (char *)boot_lua, boot_lua_size, "boot");

	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		if (Love::IsRunning())
            luaL_dostring(L, "xpcall(love.run, love.errhand)");
        else
            break;

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	Love::Exit(L);

	gfxExit();

	return 0;
}
