#pragma once

#include "common/luax.hpp"
#include "modules/sound/Decoder.hpp"

namespace love
{
    Decoder* luax_checkdecoder(lua_State* L, int index);

    int open_decoder(lua_State* L);
} // namespace love

namespace Wrap_Decoder
{
    int clone(lua_State* L);

    int getChannelCount(lua_State* L);

    int getBitDepth(lua_State* L);

    int getSampleRate(lua_State* L);

    int getDuration(lua_State* L);

    int decode(lua_State* L);

    int seek(lua_State* L);
} // namespace Wrap_Decoder
