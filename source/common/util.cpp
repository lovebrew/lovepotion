extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <map>
#include <vector>
#include <stdarg.h>

#include <switch.h>

#include "common/types.h"
#include "common/util.h"

void love_getfield(lua_State * L, const char * field)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "love");
	lua_getfield(L, -1, field);
	lua_remove(L, -2);
}

int love_preload(lua_State * L, lua_CFunction function, const char * name)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, function);
	lua_setfield(L, -2, name);
	lua_pop(L, 2);

	return 0;
}

double clamp(double low, double value, double high)
{
	return std::min(high, std::max(low, value));
}

std::map<int, std::string> LANGUAGES =
{
	{SetLanguage_JA,	"Japanese"				},
	{SetLanguage_ENUS,	"American English"		},
	{SetLanguage_FR,	"French"				},
	{SetLanguage_DE,	"German"				},
	{SetLanguage_IT,	"Italian"				},
	{SetLanguage_ES,	"Spanish"				},
	{SetLanguage_ZHCN,	"Chinese"				},
	{SetLanguage_KO,	"Korean"				},
	{SetLanguage_NL,	"Dutch"					},
	{SetLanguage_PT,	"Portuguese"			},
	{SetLanguage_RU,	"Russian"				},
	{SetLanguage_ZHTW,	"Taiwanese"				},
	{SetLanguage_ENGB,	"British English"		},
	{SetLanguage_FRCA,	"Canadian French"		},
	{SetLanguage_ES419, "Latin American Spanish"}
};

std::vector<std::string> REGIONS =
{
	"JPN",
	"USA",
	"EUR",
	"AUS",
	"CHN",
	"KOR",
	"TWN",
	"UNK" //Unknown
};

std::vector<std::string> KEYS =
{
	"a", "b", "x", "y",
	"leftstick", "rightstick",
	"l", "r", "zl", "zr",
	"plus", "minus", "dpleft",
	"dpup", "dpright", "dpdown",
	"", "", "", "", "", "", "", ""
};

std::vector<HidControllerID> CONTROLLER_IDS =
{
	CONTROLLER_PLAYER_1, CONTROLLER_PLAYER_2, 
	CONTROLLER_PLAYER_3, CONTROLLER_PLAYER_4, 
	CONTROLLER_PLAYER_5, CONTROLLER_PLAYER_6,
	CONTROLLER_PLAYER_7, CONTROLLER_PLAYER_8
};

std::vector<std::string> GAMEPAD_AXES =
{
	"leftx", "lefty",
	"rightx", "righty"
};