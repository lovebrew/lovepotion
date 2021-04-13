#pragma once

#include "objects/font/wrap_font.h"
#include "objects/text/text.h"

namespace Wrap_Text
{
    int Set(lua_State* L);

    int Setf(lua_State* L);

    int Add(lua_State* L);

    int Addf(lua_State* L);

    int Clear(lua_State* L);

    int SetFont(lua_State* L);

    int GetFont(lua_State* L);

    int GetWidth(lua_State* L);

    int GetHeight(lua_State* L);

    int GetDimensions(lua_State* L);

    love::Text* CheckText(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Text