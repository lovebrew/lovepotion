#include "love.h"
#include "common/version.h"
#include "common/runtime.h"

extern int graphicsInit(lua_State * L);
extern int filesystemInit(lua_State * L);
extern int systemInit(lua_State * L);
extern int timerInit(lua_State * L);
extern int audioInit(lua_State * L);

extern int initSourceClass(lua_State * L);
extern int initFileClass(lua_State * L);

struct { char *name; int (*fn)(lua_State *L); } modules[] = 
{
	{"graphics",	graphicsInit	},
	{"filesystem",	filesystemInit	},
	{"system",		systemInit		},
	{"timer",		timerInit		},
	{"audio",		audioInit		},
	{0}
};

int love_getVersion(lua_State * L)
{
	lua_pushinteger(L, love::VERSION_MAJOR);
	lua_pushinteger(L, love::VERSION_MINOR);
	lua_pushinteger(L, love::VERSION_REVISION);
	lua_pushstring(L, love::CODENAME);

	return 4;
}

int loveConf(lua_State * L)
{
	return 0;
}

int loveInit(lua_State * L)
{
	int (*classes[])(lua_State *L) = {
		initSourceClass,
		initFileClass,
		NULL,
	};

	for (int i = 0; classes[i]; i++) {
		classes[i](L);
		lua_pop(L, 1);
	}

	luaL_Reg reg[] = 
	{
		{ "getVersion",	love_getVersion },
		{ "conf",		loveConf		},
		{ 0, 0 },
	};

	luaL_newlib(L, reg);

	for (int i = 0; modules[i].name; i++)
	{
		modules[i].fn(L);
		lua_setfield(L, -2, modules[i].name);

		debug += "Loaded module love.";
		debug += modules[i].name;
		debug += "!\n";
	}

	return 1;
}