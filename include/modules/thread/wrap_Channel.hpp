#pragma once

#include "common/luax.hpp"
#include "modules/thread/Channel.hpp"

namespace love
{
    Channel* luax_checkchannel(lua_State* L, int index);

    int open_channel(lua_State* L);
} // namespace love

namespace Wrap_Channel
{
    int push(lua_State* L);

    int supply(lua_State* L);

    int pop(lua_State* L);

    int demand(lua_State* L);

    int peek(lua_State* L);

    int getCount(lua_State* L);

    int hasRead(lua_State* L);

    int clear(lua_State* L);

    int performAtomic(lua_State* L);
} // namespace Wrap_Channel
