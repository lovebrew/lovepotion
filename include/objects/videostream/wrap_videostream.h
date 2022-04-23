#pragma once

#include "common/luax.h"
#include "objects/videostream/videostream.h"

namespace Wrap_VideoStream
{
    int SetSync(lua_State* L);

    int GetFilename(lua_State* L);

    int Play(lua_State* L);

    int Pause(lua_State* L);

    int Seek(lua_State* L);

    int Rewind(lua_State* L);

    int Tell(lua_State* L);

    int IsPlaying(lua_State* L);

    love::VideoStream* CheckVideoStream(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_VideoStream
