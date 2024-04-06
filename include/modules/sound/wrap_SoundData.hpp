#pragma once

#include "common/luax.hpp"
#include "modules/sound/SoundData.hpp"

namespace love
{
    SoundData* luax_checksounddata(lua_State* L, int index);

    int open_sounddata(lua_State* L);
} // namespace love

namespace Wrap_SoundData
{
    int clone(lua_State* L);

    int getChannelCount(lua_State* L);

    int getBitDepth(lua_State* L);

    int getSampleRate(lua_State* L);

    int getSampleCount(lua_State* L);

    int getDuration(lua_State* L);

    int setSample(lua_State* L);

    int getSample(lua_State* L);

    int copyFrom(lua_State* L);

    int slice(lua_State* L);
} // namespace Wrap_SoundData
