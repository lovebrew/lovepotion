#pragma once

#include "common/luax.hpp"
#include "modules/image/ImageData.hpp"

namespace love
{
    ImageData* luax_checkimagedata(lua_State* L, int index);

    int open_imagedata(lua_State* L);
} // namespace love

namespace Wrap_ImageData
{
    int clone(lua_State* L);

    int getFormat(lua_State* L);

    int setLinear(lua_State* L);

    int isLinear(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);

    int getPixel(lua_State* L);

    int setPixel(lua_State* L);

    int paste(lua_State* L);

    int mapPixel(lua_State* L);

    int encode(lua_State* L);
} // namespace Wrap_ImageData
