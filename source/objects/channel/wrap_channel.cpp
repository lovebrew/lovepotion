#include "common/runtime.h"

#include "objects/channel/channel.h"
#include "objects/channel/wrap_channel.h"

#define CLASS_TYPE LUAOBJ_TYPE_CHANNEL
#define CLASS_NAME "Channel"

map<string, Channel *> channels;

int channelNew(lua_State * L)
{
    string name = "";
    if (!lua_isnoneornil(L, 1))
        name = luaL_checkstring(L, 1);

    void * raw_self = luaobj_newudata(L, sizeof(Channel));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    Channel * self;
    if (!name.empty())
        self = new (raw_self) Channel(name);
    else
        self = new (raw_self) Channel();

    channels[name] = self;

    love_register(L, 2, self);

    return 1;
}

int channelPush(lua_State * L)
{
    Channel * self = (Channel *)luaobj_checkudata(L, 1, CLASS_TYPE);

    int type = lua_type(L, 2);

    self->Push(Variant::FromLua(L, 2, type));

    return 0;
}

int channelPop(lua_State * L)
{
    Channel * self = (Channel *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Pop(L);

    return 1;
}

int channelToString(lua_State * L)
{
    Channel * self = (Channel *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString(CLASS_NAME);

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initChannelClass(lua_State * L)
{
    luaL_Reg reg[] = {
        {"new",                channelNew        },
        {"push",            channelPush        },
        {"pop",                channelPop        },
        {"__tostring",        channelToString    },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, channelNew, reg);

    return 1;
}