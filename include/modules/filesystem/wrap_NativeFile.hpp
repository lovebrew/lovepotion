#pragma once

#include "common/luax.hpp"
#include "modules/filesystem/NativeFile.hpp"

namespace love
{
    int open_nativefile(lua_State* L);
} // namespace love
