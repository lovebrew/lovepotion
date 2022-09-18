#include <common/console.hpp>
#include <common/luax.hpp>
#include <common/variant.hpp>

#include <modules/love/love.hpp>
#include <string.h>

using namespace love;

DoneAction RunLOVE(int argc, char** argv, int& retval, Variant& restartValue)
{
    /* make a new lua state */
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    /* preload "love" */
    luax::Preload(L, love::Initialize, "love");

    {
        lua_newtable(L);

        if (argc > 0)
        {
            lua_pushstring(L, argv[0]);
            lua_rawseti(L, -2, -2);
        }

        lua_pushstring(L, "embedded boot.lua");
        lua_rawseti(L, -2, -1);

        for (int i = 1; i < argc; i++)
        {
            lua_pushstring(L, argv[i]);
            lua_rawseti(L, -2, i);
        }

        lua_setglobal(L, "arg");
    }

    /* require "love" */
    lua_getglobal(L, "require");
    lua_pushstring(L, "love");
    lua_call(L, 1, 1);

    /* love.restart = value, clear it */
    luax::PushVariant(L, restartValue);
    lua_setfield(L, -2, "restart");
    restartValue = Variant();

    /* pop the love table */
    lua_pop(L, 1);

    /* boot! */
    lua_getglobal(L, "require");
    lua_pushstring(L, "love.boot");
    lua_call(L, 1, 1);

    /* put this on a new lua thread */
    lua_newthread(L);
    lua_pushvalue(L, -2);

    int stackPosition = lua_gettop(L);

    /* execute the main loop */
    while (love::MainLoop<Console::Which>(L, 0))
        lua_pop(L, lua_gettop(L) - stackPosition);

    retval          = 0;
    DoneAction done = DoneAction::DONE_QUIT;

    int returnIndex = stackPosition;
    if (!lua_isnoneornil(L, returnIndex))
    {
        if (lua_type(L, returnIndex) == LUA_TSTRING &&
            strcmp(lua_tostring(L, returnIndex), "restart") == 0)
        {
            done = DONE_RESTART;
        }

        if (lua_isnumber(L, returnIndex))
            retval = lua_tonumber(L, returnIndex);

        if (returnIndex < lua_gettop(L))
            restartValue = luax::CheckVariant(L, returnIndex + 1, false);
    }

    lua_close(L);

    return done;
}

int main(int argc, char** argv)
{
    if (love::IsRunningAppletMode<Console::Which>())
        return 0;

    DoneAction done = love::DONE_QUIT;
    int returnValue = 0;
    Variant restartValue;

    love::PreInit<Console::Which>();

    do
    {
        done = RunLOVE(argc, argv, returnValue, restartValue);
    } while (done != love::DONE_QUIT);

    love::OnExit<Console::Which>();

    return returnValue;
}
