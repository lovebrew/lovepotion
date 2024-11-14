#include "common/luax.hpp"
#include "common/version.hpp"

#include "boot.hpp"
#include "modules/love/love.hpp"

#include <vector>

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

enum DoneAction
{
    DONE_QUIT,
    DONE_RESTART
};

static DoneAction runLove(char** argv, int argc, int& result, love::Variant& restartValue)
{
    if (love::preInit() != 0)
    {
        love::onExit();
        return DONE_QUIT;
    }

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_bit(L);

    love::luax_preload(L, love_initialize, "love");

    {
        lua_newtable(L);

        if (argc > 0)
        {
            lua_pushstring(L, argv[0]);
            lua_rawseti(L, -2, -2);
        }

        // on 3DS and Switch, argv[0] is the binary path
        // on Wii U, argv[0] is "safe.rpx"
        // args[-1] is "embedded boot.lua"
        // args[1] is "game" for the game folder
        // this should allow "game" to be used in love.filesystem.setSource
        std::vector<const char*> args(argv, argv + argc);
        args.push_back("game");

        lua_pushstring(L, "embedded boot.lua");
        lua_rawseti(L, -2, -1);

        for (int index = 1; index < (int)args.size(); index++)
        {
            lua_pushstring(L, args[index]);
            lua_rawseti(L, -2, index);
        }

        lua_setglobal(L, "arg");
    }

    lua_getglobal(L, "require");
    lua_pushstring(L, "love");
    lua_call(L, 1, 1); // leave the returned table on the stack.

    {
        lua_pushboolean(L, 1);
        lua_setfield(L, -2, "_exe");
    }

    love::luax_pushvariant(L, restartValue);
    lua_setfield(L, -2, "restart");
    restartValue = love::Variant();

    lua_pop(L, 1);

    lua_getglobal(L, "require");
    lua_pushstring(L, "love.boot");
    lua_call(L, 1, 1);

    lua_newthread(L);
    lua_pushvalue(L, -2);

    int position = lua_gettop(L);
    int results  = 0;

    while (love::mainLoop(L, 0, &results))
        lua_pop(L, lua_gettop(L) - position);

    result            = 0;
    DoneAction action = DONE_QUIT;

    int index = position;
    if (!lua_isnoneornil(L, index))
    {
        if (lua_type(L, index) == LUA_TSTRING && strcmp(lua_tostring(L, index), "restart") == 0)
            action = DONE_RESTART;

        if (lua_isnumber(L, index))
            result = lua_tonumber(L, index);

        if (index < lua_gettop(L))
            restartValue = love::luax_checkvariant(L, index + 1, false);
    }

    lua_close(L);
    love::onExit();

    return action;
}

int main(int argc, char** argv)
{
    int result        = 0;
    DoneAction action = DONE_QUIT;
    love::Variant restartValue;

    do
    {
        action = runLove(argv, argc, result, restartValue);
    } while (action != DONE_QUIT);

    return result;
}
