#pragma once

#include <common/luax.hpp>

namespace Wrap_Window
{
    int SetMode(lua_State* L);

    int SetIcon(lua_State* L);

    int SetTitle(lua_State* L);

    int IsOpen(lua_State*L);

    int Register(lua_State* L);
} // namespace Wrap_Window
