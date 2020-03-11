#pragma once

#include "modules/graphics/graphics.h"
#include "modules/window/window.h"

#include "objects/font/font.h"
#include "objects/drawable/wrap_drawable.h"

#include "common/backend/primitives.h"

namespace Wrap_Graphics
{
    int Arc(lua_State * L);

    int Circle(lua_State * L);

    int Clear(lua_State * L);

    int Discard(lua_State * L);

    int Draw(lua_State * L);

    int Line(lua_State * L);

    int Points(lua_State * L);

    int Polygon(lua_State * L);

    int Present(lua_State * L);

    int Print(lua_State * L);

    int PrintF(lua_State * L);

    int Rectangle(lua_State * L);

    int SetScissor(lua_State * L);

    int NewImage(lua_State * L);

    int NewFont(lua_State * L);

    int NewQuad(lua_State * L);

    int SetDefaultFilter(lua_State * L);

    int SetLineWidth(lua_State * L);

    int SetNewFont(lua_State * L);

    int SetFont(lua_State * L);

    int Stencil(lua_State * L);

    int GetBackgroundColor(lua_State * L);

    int GetCanvas(lua_State * L);

    int GetColor(lua_State * L);

    int Reset(lua_State * L);

    int GetDefaultFilter(lua_State * L);

    int GetFont(lua_State * L);

    int GetLineWidth(lua_State * L);

    int GetScissor(lua_State * L);

    int SetBackgroundColor(lua_State * L);

    int SetColor(lua_State * L);

    int Register(lua_State * L);
}
