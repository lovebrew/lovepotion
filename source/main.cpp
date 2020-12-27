#include "common/luax.h"
#include "modules/love.h"

#if defined (_3DS)
    #include <3ds.h>
#elif defined (__SWITCH__)
    #include <switch.h>
#endif

enum DoneAction
{
    DONE_QUIT,
    DONE_RESTART
};

static bool IsApplicationType()
{
    #if defined (__SWITCH__)
        /* check for applet type */

        AppletType type = appletGetAppletType();

        AppletType appletTypeCondition = (AppletType)(AppletType_Application |
                                        AppletType_SystemApplication);

        bool isApplication = (type == appletTypeCondition);

        if (isApplication)
            return true;

        const char * TITLE_TAKEOVER_ERROR = "Please run LÖVE Potion under "
                                            "Atmosphère title takeover.";

        ErrorApplicationConfig config;

        errorApplicationCreate(&config, TITLE_TAKEOVER_ERROR, NULL);
        errorApplicationShow(&config);

        return false;
    #endif

    return true;
}

static int love_preload(lua_State *L, lua_CFunction f, const char *name)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, f);
    lua_setfield(L, -2, name);
    lua_pop(L, 2);

    return 0;
}

DoneAction Run_Love_Potion(int argc, char ** argv, int & retval)
{
    // Make a new Lua state
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);

    // preload love
    love_preload(L, Love::Initialize, "love");

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

    /*
    ** get what's on the stack
    ** this will keep running until "quit"
    */
    int stackpos = lua_gettop(L);

    #if defined (_3DS)
        while (Luax::Resume(L, 0) == LUA_YIELD && aptMainLoop())
    #elif defined (__SWITCH__)
        while (Luax::Resume(L, 0) == LUA_YIELD)
    #endif
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
    if (!IsApplicationType())
        return 0;

    DoneAction done = DONE_QUIT;
    int retval = 0;

    do
    {
        done = Run_Love_Potion(argc, argv, retval);
    } while (done != DoneAction::DONE_QUIT);

    return retval;
}
