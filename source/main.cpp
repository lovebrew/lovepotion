#include <common/console.hpp>
#include <common/luax.hpp>

#include <modules/filesystem/filesystem.hpp>
#include <modules/love/love.hpp>

#include <string.h>

using namespace love;

DoneAction RunLOVE(int argc, char** argv, int& retval)
{
    // Make a new Lua state
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    // preload love
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

    // require "love"
    lua_getglobal(L, "require");
    lua_pushstring(L, "love");
    lua_call(L, 1, 1);
    lua_pop(L, 1);

    // boot!
    lua_getglobal(L, "require");
    lua_pushstring(L, "love.boot");
    lua_call(L, 1, 1);

    // put this on a new lua thread
    lua_newthread(L);
    lua_pushvalue(L, -2);

    int stackPosition = lua_gettop(L);

    /* execute the main loop */
    while (love::MainLoop<love::Console::Which>(L, 0))
        lua_pop(L, lua_gettop(L) - stackPosition);

    retval          = 0;
    DoneAction done = DoneAction::DONE_QUIT;

    // if we wish to "restart", start up again after closing
    if (lua_type(L, -1) == LUA_TSTRING && strcmp(lua_tostring(L, -1), "restart") == 0)
        done = DoneAction::DONE_RESTART;

    // custom quit value
    if (lua_isnumber(L, -1))
        retval = (int)lua_tonumber(L, -1);

    lua_close(L);

    // actually return quit
    return done;
}

int main(int argc, char** argv)
{
    if (love::IsRunningAppletMode<Console::Which>())
        return 0;

    DoneAction done = love::DONE_QUIT;
    int returnValue = 0;

    love::PreInit<Console::Which>();

    do
    {
        done = RunLOVE(argc, argv, returnValue);
    } while (done != love::DONE_QUIT);

    love::OnExit<Console::Which>();

    return 0;
}
