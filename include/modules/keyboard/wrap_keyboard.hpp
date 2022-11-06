#pragma once

#include <common/luax.hpp>

namespace Wrap_Keyboard
{
    int SetTextInput(lua_State* L);

    int HasTextInput(lua_State* L);

    int HasScreenKeyboard(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Keyboard
