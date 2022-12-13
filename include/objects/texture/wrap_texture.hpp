#pragma once

#include <common/luax.hpp>
#include <objects/texture_ext.hpp>

namespace Wrap_Texture
{
    love::Texture<love::Console::Which>* CheckTexture(lua_State* L, int index);

    int GetTextureType(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetDepth(lua_State* L);

    int GetLayerCount(lua_State* L);

    int GetMipmapCount(lua_State* L);

    int GetPixelWidth(lua_State* L);

    int GetPixelHeight(lua_State* L);

    int GetPixelDimensions(lua_State* L);

    int GetDPIScale(lua_State* L);

    int IsFormatLinear(lua_State* L);

    int IsCompressed(lua_State* L);

    int GetMSAA(lua_State* L);

    int SetFilter(lua_State* L);

    int GetFilter(lua_State* L);

    int SetMipmapFilter(lua_State* L);

    int GetMipmapFilter(lua_State* L);

    int SetWrap(lua_State* L);

    int GetWrap(lua_State* L);

    int GetFormat(lua_State* L);

    int IsRenderTarget(lua_State* L);

    int IsComputeWritable(lua_State* L);

    int IsReadable(lua_State* L);

    int SetDepthSampleMode(lua_State* L);

    int GetDepthSampleMode(lua_State* L);

    int GetMipmapMode(lua_State* L);

    int GenerateMipmaps(lua_State* L);

    int ReplacePixels(lua_State* L);

    int RenderTo(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Texture
