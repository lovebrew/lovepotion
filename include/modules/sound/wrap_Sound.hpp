#pragma once

#include "common/luax.hpp"
#include "modules/sound/Sound.hpp"

namespace Wrap_Sound
{
    int newDecoder(lua_State* L);

    int newSoundData(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Sound
