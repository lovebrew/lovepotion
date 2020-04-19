#include "common/runtime.h"
#include "common/assets.h"

#include "modules/love.h"

DoneAction Run_Love_Potion(int & retval)
{
    // Make a new Lua state
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);

    // preload love
    Love::Preload(L, Love::Initialize, "love");

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

    /*
    ** get what's on the stack
    ** this will keep running until "quit"
    */
    int stackpos = lua_gettop(L);
    while (lua_resume(L, nullptr, 0) == LUA_YIELD)
            lua_pop(L, lua_gettop(L) - stackpos);

    retval = 0;
    DoneAction done = DONE_QUIT;

    // if we wish to "restart", start up again after closing
    if (lua_type(L, -1) == LUA_TSTRING && strcmp(lua_tostring(L, -1), "restart") == 0)
        done = DONE_RESTART;

    // custom quit value
    if (lua_isnumber(L, -1))
        retval = (int)lua_tonumber(L, -1);

    lua_close(L);

    // actually return quit
    return done;
}

int main(int argc, char * argv[])
{
    #if defined (_3DS)
        userAppInit();
    #endif

    std::string path;

    if (argc > 0)
        path = (argc == 2) ? argv[1] : argv[0];

    Assets::Initialize(path);

    DoneAction done = DONE_QUIT;
    int retval = 0;

    while (appletMainLoop())
    {
        done = Run_Love_Potion(retval);
        if (done == DoneAction::DONE_QUIT)
            break;
    }

    #if defined (_3DS)
        userAppExit();
    #endif

    return retval;
}
