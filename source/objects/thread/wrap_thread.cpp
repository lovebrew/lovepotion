#include "common/runtime.h"
#include "modules/mod_thread.h"

#include "objects/thread/thread.h"

#define CLASS_TYPE LUAOBJ_TYPE_THREAD
#define CLASS_NAME "Thread"

int threadNew(lua_State * L)
{
    const char * arg = luaL_checkstring(L, 1);

    void * raw_self = luaobj_newudata(L, sizeof(ThreadClass));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    new (raw_self) ThreadClass(arg);

    return 1;
}

int threadStart(lua_State * L)
{
    ThreadClass * self = (ThreadClass *)luaobj_checkudata(L, 1, CLASS_TYPE);

    uint argc = lua_gettop(L) - 1;

    vector<Variant> args;
    args.reserve(argc);

    for (uint i = 0; i < argc; ++i)
    {
        if (!lua_isnone(L, i + 2))
            args.push_back(Variant::FromLua(L, i + 2));
    }

    if (!self->IsRunning())
        self->Start(args);

    return 0;
}

int threadWait(lua_State * L)
{
    ThreadClass * self = (ThreadClass *)luaobj_checkudata(L, 1, CLASS_TYPE);

    if (self->IsRunning())
        self->Wait();

    return 0;
}

int threadGetError(lua_State * L)
{
    ThreadClass * self = (ThreadClass *)luaobj_checkudata(L, 1, CLASS_TYPE);

    string error = self->GetError();
    lua_pushstring(L, error.c_str());

    return 1;
}

int threadIsRunning(lua_State * L)
{
    ThreadClass * self = (ThreadClass *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushboolean(L, self->IsRunning());

    return 1;
}

int threadToString(lua_State * L)
{
    ThreadClass * self = (ThreadClass *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initThreadClass(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "__tostring", threadToString  },
        { "getError",   threadGetError  },
        { "isRunning",  threadIsRunning },
        { "new",        threadNew       },
        { "start",      threadStart     },
        { "wait",       threadWait      },
        { 0, 0 }
    };

    luaobj_newclass(L, CLASS_NAME, NULL, threadNew, reg);

    return 1;
}