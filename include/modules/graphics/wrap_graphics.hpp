#pragma once

#include <common/luax.hpp>

#include <common/matrix.tcc>
#include <objects/transform/transform.hpp>

namespace Wrap_Graphics
{
    template<typename T>
    static void CheckStandardTransform(lua_State* L, int index, const T& func)
    {
        if (luax::IsType(L, index, love::Transform::type))
        {
            love::Transform* transform = luax::ToType<love::Transform>(L, index);
            func(transform->GetMatrix());
        }
        else
        {
            float x  = luaL_optnumber(L, index + 0, 0.0);
            float y  = luaL_optnumber(L, index + 1, 0.0);
            float a  = luaL_optnumber(L, index + 2, 0.0);
            float sx = luaL_optnumber(L, index + 3, 1.0);
            float sy = luaL_optnumber(L, index + 4, sx);
            float ox = luaL_optnumber(L, index + 5, 0.0);
            float oy = luaL_optnumber(L, index + 6, 0.0);
            float kx = luaL_optnumber(L, index + 7, 0.0);
            float ky = luaL_optnumber(L, index + 8, 0.0);
            func(love::Matrix4(x, y, a, sx, sy, ox, oy, kx, ky));
        }
    }

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

    int SetMeshCullMode(lua_State* L);

    int GetMeshCullMode(lua_State* L);

    int SetFrontFaceWinding(lua_State* L);

    int GetFrontFaceWinding(lua_State* L);

    int SetDefaultFilter(lua_State* L);

    int GetDefaultFilter(lua_State* L);

    int SetDefaultMipmapFilter(lua_State* L);

    int GetDefaultMipmapFilter(lua_State* L);

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

    /* NINTENDO 3DS */

    int Get3D(lua_State* L);

    int Set3D(lua_State* L);

    int GetDepth(lua_State* L);

    /* OBJECTS */

    int NewFont(lua_State* L);

    int SetFont(lua_State* L);

    int GetFont(lua_State* L);

    int Draw(lua_State* L);

    int NewTexture(lua_State* L);

    int NewImage(lua_State* L);

    int NewCanvas(lua_State* L);

    int SetCanvas(lua_State* L);

    int GetCanvas(lua_State* L);

    int NewQuad(lua_State* L);

    int NewTextBatch(lua_State* L);

    int NewSpriteBatch(lua_State* L);

    int NewMesh(lua_State* L);

    int Print(lua_State* L);

    int Printf(lua_State* L);

    /* PRIMITIVES */

    int Rectangle(lua_State* L);

    int Circle(lua_State* L);

    int Line(lua_State* L);

    int Arc(lua_State* L);

    int Points(lua_State* L);

    int Ellipse(lua_State* L);

    int Polygon(lua_State* L);

    /* OTHER STUFF */

    int GetRendererInfo(lua_State* L);

    int GetStats(lua_State* L);

    int Push(lua_State* L);

    int Translate(lua_State* L);

    int Scale(lua_State* L);

    int Shear(lua_State* L);

    int Rotate(lua_State* L);

    int ApplyTransform(lua_State* L);

    int InverseTransformPoint(lua_State* L);

    int TransformPoint(lua_State* L);

    int ReplaceTransform(lua_State* L);

    int Pop(lua_State* L);

    int Register(lua_State* L);

    extern std::span<const luaL_Reg> extensions;
} // namespace Wrap_Graphics
