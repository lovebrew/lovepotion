#pragma once

#include <common/luax.hpp>

#include <objects/font/wrap_font.hpp>
#include <objects/textbatch_ext.hpp>
#include <objects/transform/wrap_transform.hpp>

namespace Wrap_TextBatch
{
    love::TextBatch<love::Console::Which>* CheckTextBatch(lua_State* L, int index);

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

    int Register(lua_State* L);
} // namespace Wrap_TextBatch
