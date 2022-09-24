#pragma once

#include <common/luax.hpp>

namespace Wrap_Audio
{
    int GetActiveSourceCount(lua_State* L);

    int NewSource(lua_State* L);

    int NewQueueableSource(lua_State* L);

    int Play(lua_State* L);

    int Stop(lua_State* L);

    int Pause(lua_State* L);

    int SetVolume(lua_State* L);

    int GetVolume(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Audio
