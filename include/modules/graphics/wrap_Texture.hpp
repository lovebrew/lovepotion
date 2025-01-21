#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Texture.hpp"

namespace love
{
    TextureBase* luax_checktexture(lua_State* L, int index);

    int open_texture(lua_State* L);
} // namespace love

namespace Wrap_Texture
{
    int getTextureType(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);

    int getDepth(lua_State* L);

    int getLayerCount(lua_State* L);

    int getMipmapCount(lua_State* L);

    int getPixelWidth(lua_State* L);

    int getPixelHeight(lua_State* L);

    int getPixelDimensions(lua_State* L);

    int getDPIScale(lua_State* L);

    int isFormatLinear(lua_State* L);

    int isCompressed(lua_State* L);

    int getMSAA(lua_State* L);

    int setFilter(lua_State* L);

    int getFilter(lua_State* L);

    int setMipmapFilter(lua_State* L);

    int getMipmapFilter(lua_State* L);

    int setWrap(lua_State* L);

    int getWrap(lua_State* L);

    int getFormat(lua_State* L);

    int isCanvas(lua_State* L);

    int isComputeWritable(lua_State* L);

    int isReadable(lua_State* L);

    int getViewFormats(lua_State* L);

    int getMipmapMode(lua_State* L);

    int getDepthSampleMode(lua_State* L);

    int setDepthSampleMode(lua_State* L);

    int generateMipmaps(lua_State* L);

    int replacePixels(lua_State* L);

    int renderTo(lua_State* L);

    int getDebugName(lua_State* L);
} // namespace Wrap_Texture
