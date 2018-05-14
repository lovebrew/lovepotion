#include "runtime.h"
#include <unistd.h>

#include "love.h"
#include "version.h"

#include "audio.h"
#include "event.h"
#include "filesystem.h"
#include "graphics.h"
#include "keyboard.h"
#include "mouse.h"
#include "system.h"
#include "timer.h"
#include "window.h"

#include "wrap_font.h"
#include "wrap_image.h"
#include "wrap_quad.h"
#include "wrap_source.h"
#include "wrap_file.h"

struct { const char * name; int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
	{ "audio",		Audio::Register,		Audio::Exit			},
	{ "event",		Event::Register,		NULL				},
	{ "graphics",	Graphics::Register,		Graphics::Exit		},
	{ "filesystem",	Filesystem::Register,	NULL				},
	{ "keyboard",	Keyboard::Register,		NULL				},
	{ "mouse",		Mouse::Register, 		NULL				},
	{ "timer",		Timer::Register, 		NULL				},
	{ "system",		System::Register, 		System::Exit		},
	{ "window",		Window::Register, 		NULL				},
	{ 0 }
};

int Love::Initialize(lua_State * L)
{
	int (*classes[])(lua_State *L) = 
	{
		initFileClass,
		initFontClass,
		initImageClass,
		initQuadClass,
		initSourceClass,
		NULL
	};

	for (int i = 0; classes[i]; i++) 
	{
		classes[i](L);
		lua_pop(L, 1);
	}

	luaL_Reg reg[] =
	{
		{ "enableConsole",	EnableConsole	},
		{ "getVersion",		GetVersion		},
		{ 0, 0 }
	};

	luaL_newlib(L, reg);

	for (int i = 0; modules[i].name; i++)
	{
		modules[i].fn(L);
		lua_setfield(L, -2, modules[i].name);
	}

	return 1;
}

int Love::GetVersion(lua_State * L)
{
	lua_pushnumber(L, Love::VERSION_MAJOR);
	lua_pushnumber(L, Love::VERSION_MINOR);
	lua_pushnumber(L, Love::VERSION_REVISION);
	lua_pushstring(L, Love::CODENAME.c_str());

	return 4;
}

int Love::EnableConsole(lua_State * L)
{
	lua_Debug info;
	int level = 0;

	bool canEnable = false;
	while (lua_getstack(L, level, &info))
	{
		lua_getinfo(L, "nfSl", &info);
		if (strncmp(info.short_src, "[string \"boot\"]", 15) == 0)
			canEnable = true;

		++level;
	}

	if (canEnable)
		Console::Initialize(GFX_BOTTOM);

	return 0;
}

int Love::NoGame(lua_State * L)
{
	chdir("romfs:/");

	if (luaL_dofile(L, "main.lua"))
		Console::ThrowError(L);
}

void Love::Exit(lua_State * L)
{
	for (int i = 0; modules[i].close; i++)
		modules[i].close();

	lua_close(L);
}