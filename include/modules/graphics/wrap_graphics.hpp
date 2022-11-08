#pragma once

#include <common/luax.hpp>

namespace Wrap_Graphics
{
    int Reset(lua_State* L);

    int Clear(lua_State* L);

    int Present(lua_State* L);

    int IsCreated(lua_State* L);

    int IsActive(lua_State* L);

    int Origin(lua_State* L);

    int IsGammaCorrect(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    int GetPixelWidth(lua_State* L);

    int GetPixelHeight(lua_State* L);

    int GetPixelDimensions(lua_State* L);

    int GetDPIScale(lua_State* L);

    /* todo: SetCanvas */

    /* todo: GetCanvas */

    int SetScissor(lua_State* L);

    int IntersectScissor(lua_State* L);

    int GetScissor(lua_State* L);

    int SetColor(lua_State* L);

    int GetColor(lua_State* L);

    int SetBackgroundColor(lua_State* L);

    int GetBackgroundColor(lua_State* L);

    int SetColorMask(lua_State* L);

    int GetColorMask(lua_State* L);

    int SetBlendMode(lua_State* L);

    int GetBlendMode(lua_State* L);

    int SetBlendState(lua_State* L);

    int GetBlendState(lua_State* L);

    int SetDefaultFilter(lua_State* L);

    int GetDefaultFilter(lua_State* L);

    int SetDefaultMipmapFilter(lua_State* L);

    int GetDefaultsMipmapFilter(lua_State* L);

    int SetLineWidth(lua_State* L);

    int GetLineWidth(lua_State* L);

    int SetLineStyle(lua_State* L);

    int GetLineStyle(lua_State* L);

    int SetLineJoin(lua_State* L);

    int GetLineJoin(lua_State* L);

    int SetPointSize(lua_State* L);

    int GetPointSize(lua_State* L);

    int SetActiveScreen(lua_State* L);

    int GetScreens(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Graphics
