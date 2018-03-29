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

AudioType GetAudioType(const std::string & path)
{
	size_t pos = path.find(".");
	std::string split = path.substr(pos + 1);

	if (split == "wav")
		return AUDIO_WAV;
	else if (split == "ogg")
		return AUDIO_OGG;

	return AUDIO_INVALID;
}

double clamp(double low, double value, double high)
{
	return std::min(high, std::max(low, value));
}

void logToFile(const std::string & data)
{
	fwrite((char *)data.data(), 1, data.length(), logFile);

	fflush(logFile);
}

void closeLog()
{
	fclose(logFile);
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