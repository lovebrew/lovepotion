#pragma once

#include <common/luax.hpp>

namespace Wrap_Sound
{
    int NewDecoder(lua_State* L);

    int NewSoundData(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Sound
