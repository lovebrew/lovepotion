#pragma once

#include "common/luax.h"
#include "modules/touch/touch.h"

namespace Wrap_Touch
{
    int GetTouches(lua_State* L);

    int GetPosition(lua_State* L);

    int GetPressure(lua_State* L);

    int64_t CheckTouchID(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Touch
