extern "C" 
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.3.h>
    #include <luaobj.h>
}

#include <map>
#include <vector>
#include <stdarg.h>

#include <switch.h>

#include "common/types.h"
#include "common/variant.h"
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

void love_register(lua_State * L, int index, void * object)
{
    love_get_registry(L, OBJECTS);

    lua_pushlightuserdata(L, object);    //light userdata key
    lua_pushvalue(L, index);                //push the userdata value to the key
    lua_settable(L, -3);                //set the taaable (╯°□°）╯︵ ┻━┻

    lua_setfield(L, LUA_REGISTRYINDEX, "_loveobjects");

    //lua_pop(L, 1);
    printf("Registered %p to _loveobjects!\n", object);
}

void love_push_userdata(lua_State * L, void * object)
{
    if (!object)
        return;

    love_get_registry(L, OBJECTS);
    lua_pushlightuserdata(L, object);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}

bool love_is_registered(lua_State * L, void * object)
{
    bool valid = true;

    if (!object)
        return false;

    love_get_registry(L, OBJECTS);
    lua_pushlightuserdata(L, object);
    lua_gettable(L, -2);
    lua_remove(L, -2);

    if (lua_isnil(L, 2))
        valid = false;

    lua_pop(L, 1);

    return valid;
}

int love_get_registry(lua_State * L, REGISTRY registry)
{
    switch(registry)
    {
        case OBJECTS:
            lua_getfield(L, LUA_REGISTRYINDEX, "_loveobjects");
            return 1;
        default:
            return luaL_error(L, "Attempted to use invalid registry."); 
    }
}

int luax_traceback(lua_State *L)
{
    if (!lua_isstring(L, 1))  // 'message' not a string?
        return 1; // keep it intact

    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return 1;
    }

    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 2);
        return 1;
    }

    lua_pushvalue(L, 1); // pass error message
    lua_pushinteger(L, 2); // skip this function and traceback
    lua_call(L, 2, 1); // call debug.traceback
    return 1;
}

double clamp(double low, double value, double high)
{
    return std::min(high, std::max(low, value));
}

std::map<int, std::string> LANGUAGES =
{
    {SetLanguage_JA,    "Japanese"               },   
    {SetLanguage_ENUS,  "American English"       }, 
    {SetLanguage_FR,    "French"                 },     
    {SetLanguage_DE,    "German"                 },     
    {SetLanguage_IT,    "Italian"                },    
    {SetLanguage_ES,    "Spanish"                },    
    {SetLanguage_ZHCN,  "Chinese"                }, 
    {SetLanguage_KO,    "Korean"                 },     
    {SetLanguage_NL,    "Dutch"                  },  
    {SetLanguage_PT,    "Portuguese"             },     
    {SetLanguage_RU,    "Russian"                },    
    {SetLanguage_ZHTW,  "Taiwanese"              },
    {SetLanguage_ENGB,  "British English"        },  
    {SetLanguage_FRCA,  "Canadian French"        },  
    {SetLanguage_ES419, "Latin American Spanish" }      
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
    "", "", "", "", "", "", "", "",
    "sl", "sr", "sl", "sr"
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