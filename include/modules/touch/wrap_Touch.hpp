#pragma once

#include "common/luax.hpp"
#include "modules/touch/Touch.hpp"

namespace love
{
    int64_t luax_checktouchid(lua_State* L, int index);
}

namespace Wrap_Touch
{
    int getTouches(lua_State* L);

    int getPosition(lua_State* L);

    int getPressure(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Touch
