extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <map>
#include <stdarg.h>

#include <switch.h>

void love_getfield(lua_State * L, const char * field)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "love");
	lua_getfield(L, -1, field);
	lua_remove(L, -2);
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