#pragma once

#include "objects/sounddata/sounddata.h"

namespace Wrap_SoundData
{
    int Clone(lua_State * L);

    int GetBitDepth(lua_State * L);

    int GetChannelCount(lua_State * L);

    int GetDuration(lua_State * L);

    int GetSampleCount(lua_State * L);

    int GetSampleRate(lua_State * L);

    love::SoundData * CheckSoundData(lua_State * L, int index);

    int Register(lua_State * L);
}
