#pragma once

#include "common/luax.hpp"
#include "modules/audio/Audio.hpp"

namespace Wrap_Audio
{
    int newSource(lua_State* L);

    int play(lua_State* L);

    int pause(lua_State* L);

    int stop(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Audio
