#pragma once

#include "common/luax.h"
#include "objects/decoder/decoder.h"

namespace Wrap_Decoder
{
    int Clone(lua_State* L);

    int GetBitDepth(lua_State* L);

    int GetChannelCount(lua_State* L);

    int GetDuration(lua_State* L);

    int GetSampleRate(lua_State* L);

    int Decode(lua_State* L);

    int Seek(lua_State* L);

    love::Decoder* CheckDecoder(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Decoder
