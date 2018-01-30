#include "runtime.h"

#include "love.h"
#include "version.h"

#include "graphics.h"

#include <switch.h>

struct { const char * name; int (*fn)(lua_State *L); void (*close)(void); } modules[] = 
{
	{ "graphics",	Graphics::Register,		Graphics::Exit		},
	{ 0 }
};

int Love::Initialize(lua_State * L)
{
	int (*classes[])(lua_State *L) = 
	{
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

	return 0;
}

void Love::Exit(lua_State * L)
{
	for (int i = 0; modules[i].close; i++)
		modules[i].close();

	lua_close(L);
}