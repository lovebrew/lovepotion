#pragma once

#include "common/luax.h"
#include "modules/keyboard/keyboard.h"

namespace Wrap_Keyboard
{
    int SetTextInput(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Keyboard
