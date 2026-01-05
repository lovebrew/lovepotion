#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Video.hpp"

namespace love
{
    Video* luax_checkvideo(lua_State* L, int index);

    int open_video(lua_State* L);
} // namespace love

namespace Wrap_Video
{
    int getStream(lua_State* L);

    int getSource(lua_State* L);

    int setSource(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);

    int getPixelWidth(lua_State* L);

    int getPixelHeight(lua_State* L);

    int getPixelDimensions(lua_State* L);

    int setFilter(lua_State* L);

    int getFilter(lua_State* L);
} // namespace Wrap_Video
