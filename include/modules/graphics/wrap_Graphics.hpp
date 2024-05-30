#pragma once

#include "common/luax.hpp"
#include "modules/graphics/Graphics.hpp"

#include "modules/math/Transform.hpp"

namespace love
{
    template<typename T>
    static void luax_checkstandardtransform(lua_State* L, int index, const T& func)
    {
        /* TODO: check Transform type */

        auto* transform = luax_totype<Transform>(L, index);

        if (transform != nullptr)
            func(transform->getMatrix());
        else
        {
            int nargs = lua_gettop(L);

            float x  = luaL_optnumber(L, index + 0, 0.0);
            float y  = luaL_optnumber(L, index + 1, 0.0);
            float a  = luaL_optnumber(L, index + 2, 0.0);
            float sx = luaL_optnumber(L, index + 3, 1.0);
            float sy = luaL_optnumber(L, index + 4, sx);

            float ox = 0.0f;
            float oy = 0.0f;

            if (nargs >= index + 5)
            {
                ox = luaL_optnumber(L, index + 5, 0.0);
                oy = luaL_optnumber(L, index + 6, 0.0);
            }

            float kx = 0.0f;
            float ky = 0.0f;

            if (nargs >= index + 7)
            {
                kx = luaL_optnumber(L, index + 7, 0.0);
                ky = luaL_optnumber(L, index + 8, 0.0);
            }

            func(love::Matrix4(x, y, a, sx, sy, ox, oy, kx, ky));
        }
    }
} // namespace love

namespace Wrap_Graphics
{
    int reset(lua_State* L);

    int clear(lua_State* L);

    int present(lua_State* L);

    int setColor(lua_State* L);

    int getColor(lua_State* L);

    int setBackgroundColor(lua_State* L);

    int getBackgroundColor(lua_State* L);

    int setColorMask(lua_State* L);

    int getColorMask(lua_State* L);

    int setBlendMode(lua_State* L);

    int getBlendMode(lua_State* L);

    int setBlendState(lua_State* L);

    int getBlendState(lua_State* L);

    int setLineWidth(lua_State* L);

    int getLineWidth(lua_State* L);

    int setLineJoin(lua_State* L);

    int getLineJoin(lua_State* L);

    int setLineStyle(lua_State* L);

    int getLineStyle(lua_State* L);

    int setPointSize(lua_State* L);

    int getPointSize(lua_State* L);

    int setMeshCullMode(lua_State* L);

    int getMeshCullMode(lua_State* L);

    int setFrontFaceWinding(lua_State* L);

    int getFrontFaceWinding(lua_State* L);

    int getTextureFormats(lua_State* L);

    int getSystemLimits(lua_State* L);

    int getTextureTypes(lua_State* L);

    int getStats(lua_State* L);

    int isCreated(lua_State* L);

    int isActive(lua_State* L);

    int isGammaCorrect(lua_State* L);

    int getRendererInfo(lua_State* L);

    int getWidth(lua_State* L);

    int getHeight(lua_State* L);

    int getDimensions(lua_State* L);

    int getPixelWidth(lua_State* L);

    int getPixelHeight(lua_State* L);

    int getPixelDimensions(lua_State* L);

    int getDPIScale(lua_State* L);

    int setScissor(lua_State* L);

    int intersectScissor(lua_State* L);

    int getScissor(lua_State* L);

    int getStackDepth(lua_State* L);

    int push(lua_State* L);

    int pop(lua_State* L);

    int rotate(lua_State* L);

    int scale(lua_State* L);

    int translate(lua_State* L);

    int shear(lua_State* L);

    int origin(lua_State* L);

    int applyTransform(lua_State* L);

    int replaceTransform(lua_State* L);

    int transformPoint(lua_State* L);

    int inverseTransformPoint(lua_State* L);

    int newTexture(lua_State* L);

    int newImage(lua_State* L);

    int newCanvas(lua_State* L);

    int setFont(lua_State* L);

    int getFont(lua_State* L);

    int polygon(lua_State* L);

    int rectangle(lua_State* L);

    int circle(lua_State* L);

    int ellipse(lua_State* L);

    int arc(lua_State* L);

    int points(lua_State* L);

    int draw(lua_State* L);

    int newTextBatch(lua_State* L);

    int newFont(lua_State* L);

    int print(lua_State* L);

    int printf(lua_State* L);

    int getScreens(lua_State* L);

    int setActiveScreen(lua_State* L);

    int is3D(lua_State* L);

    int set3D(lua_State* L);

    int isWide(lua_State* L);

    int setWide(lua_State* L);

    int getDepth(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Graphics
