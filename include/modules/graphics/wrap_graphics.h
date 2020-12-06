#pragma once

#include "modules/graphics/graphics.h"

#if defined (__SWITCH__)
    #include "deko3d/graphics.h"
#elif defined (_3DS)
    #include "citro2d/graphics.h"
#endif

#include "modules/window/window.h"

#include "objects/font/font.h"
#include "objects/drawable/wrap_drawable.h"

namespace Wrap_Graphics
{
    int Arc(lua_State * L);

    int Circle(lua_State * L);

    int Clear(lua_State * L);

    int Discard(lua_State * L);

    int Draw(lua_State * L);

    int Ellipse(lua_State * L);

    int Line(lua_State * L);

    int GetPointSize(lua_State * L);

    int SetPointSize(lua_State * L);

    int Points(lua_State * L);

    int Polygon(lua_State * L);

    int Present(lua_State * L);

    int Push(lua_State * L);

    int Origin(lua_State * L);

    int Translate(lua_State * L);

    int Scale(lua_State * L);

    int Rotate(lua_State * L);

    int Shear(lua_State * L);

    int Pop(lua_State * L);

    int Print(lua_State * L);

    int PrintF(lua_State * L);

    int Rectangle(lua_State * L);

    int SetScissor(lua_State * L);

    int IntersectScissor(lua_State * L);

    int ApplyTransform(lua_State * L);

    int ReplaceTransform(lua_State * L);

    int TransformPoint(lua_State * L);

    int InverseTransformPoint(lua_State * L);

    int GetScissor(lua_State * L);

    int NewImage(lua_State * L);

    int NewFont(lua_State * L);

    int NewQuad(lua_State * L);

    int NewText(lua_State * L);

    int NewCanvas(lua_State * L);

    int SetDefaultFilter(lua_State * L);

    int SetLineWidth(lua_State * L);

    int SetNewFont(lua_State * L);

    int SetFont(lua_State * L);

    int Stencil(lua_State * L);

    int GetRendererInfo(lua_State * L);

    int GetBackgroundColor(lua_State * L);

    int GetCanvas(lua_State * L);

    int GetColor(lua_State * L);

    int Reset(lua_State * L);

    int GetDefaultFilter(lua_State * L);

    int GetFont(lua_State * L);

    int GetLineWidth(lua_State * L);

    int GetScissor(lua_State * L);

    int GetWidth(lua_State * L);

    int GetHeight(lua_State * L);

    int GetDimensions(lua_State * L);

    int SetBackgroundColor(lua_State * L);

    int SetColor(lua_State * L);

    int SetCanvas(lua_State * L);

    int GetStereoscopicDepth(lua_State * L);

    int Register(lua_State * L);
}
