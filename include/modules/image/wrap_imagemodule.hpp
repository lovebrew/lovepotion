#pragma once

#include <common/luax.hpp>

namespace Wrap_ImageModule
{
    int NewImageData(lua_State* L);

    int NewCompressedData(lua_State* L);

    int IsCompressed(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_ImageModule
