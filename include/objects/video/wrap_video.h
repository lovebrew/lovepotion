#pragma once

#include "common/luax.h"
#include "objects/video/video.h"

namespace Wrap_Video
{
    int GetStream(lua_State* L);

    int GetSource(lua_State* L);

    int SetSource(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetPixelWidth(lua_State* L);

    int GetPixelHeight(lua_State* L);

    int GetPixelDimensions(lua_State* L);

    int SetFilter(lua_State* L);

    int GetFilter(lua_State* L);

    love::Video* CheckVideo(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Video
