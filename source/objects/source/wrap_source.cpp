#include "common/runtime.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "objects/source/source.h"
#include "objects/source/wrap_source.h"

#define CLASS_TYPE LUAOBJ_TYPE_SOURCE
#define CLASS_NAME "Source"

std::map<int, Source *> streams;

int sourceNew(lua_State * L)
{
    const char * path = luaL_checkstring(L, 1);

    LOVE_VALIDATE_FILE_EXISTS(path);

    const char * type = luaL_checkstring(L, 2);

    LOVE_VALIDATE_SOURCE_TYPE(type);

    void * raw_self = luaobj_newudata(L, sizeof(Source));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    Source * self = new (raw_self) Source(path, type);

    return 1;
}

//Source:play
int sourcePlay(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Play();

    return 0;
}

//Source:stop
int sourceStop(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Stop();

    return 0;
}

//Source:setLooping
int sourceSetLooping(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    bool loop = false;
    if (lua_isboolean(L, 2))
        loop = lua_toboolean(L, 2);

    self->SetLooping(loop);

    return 0;
}

//Source:isPlaying
int sourceIsPlaying(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushboolean(L, self->IsPlaying());

    return 1;
}

//Source:isLooping
int sourceIsLooping(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushboolean(L, self->IsLooping());

    return 1;
}

int sourceToString(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int sourceGC(lua_State * L)
{
    Source * self = (Source *)luaobj_checkudata(L, 1, CLASS_TYPE);

    if (!self->IsPlaying())
        self->~Source();

    return 0;
}

int initSourceClass(lua_State *L) 
{
    luaL_Reg reg[] = 
    {
        { "__gc",       sourceGC         },
        { "__tostring", sourceToString   },
        { "isLooping",  sourceIsLooping  },
        { "isPlaying",  sourceIsPlaying  },
        { "new",        sourceNew        },
        { "play",       sourcePlay       },
        { "setLooping", sourceSetLooping },
        { "stop",       sourceStop       },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, sourceNew, reg);

    return 1;
}