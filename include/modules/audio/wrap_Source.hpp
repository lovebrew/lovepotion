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

    int stop(lua_State* L);

    int pause(lua_State* L);

    int setPitch(lua_State* L);

    int getPitch(lua_State* L);

    int setVolume(lua_State* L);

    int getVolume(lua_State* L);

    int seek(lua_State* L);

    int tell(lua_State* L);

    int getDuration(lua_State* L);

    int setLooping(lua_State* L);

    int isLooping(lua_State* L);

    int isPlaying(lua_State* L);

    int setVolumeLimits(lua_State* L);

    int getVolumeLimits(lua_State* L);

    int getChannelCount(lua_State* L);

    int getFreeBufferCount(lua_State* L);

    int getType(lua_State* L);
} // namespace Wrap_Source
