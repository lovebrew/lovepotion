#pragma once

#include "common/luax.hpp"
#include "modules/audio/Source.hpp"

namespace love
{
    Source* luax_checksource(lua_State* L, int index);

    int open_source(lua_State* L);
} // namespace love

namespace Wrap_Source
{
    int clone(lua_State* L);

    int play(lua_State* L);

    int pause(lua_State* L);

    int stop(lua_State* L);

    int setLooping(lua_State* L);

    int tell(lua_State* L);
} // namespace Wrap_Source
