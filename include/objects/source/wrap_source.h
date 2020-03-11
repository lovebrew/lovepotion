#pragma once

#include "objects/source/source.h"

namespace Wrap_Source
{
    love::Source * CheckSource(lua_State * L, int index);

    int Register(lua_State * L);
}
