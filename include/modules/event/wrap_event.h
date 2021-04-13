#pragma once

#include "common/luax.h"
#include "modules/event/event.h"
namespace Wrap_Event
{
    int Clear(lua_State* L);

    int Pump(lua_State* L);

    int Push(lua_State* L);

    int Quit(lua_State* L);

    int Wait(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Event
