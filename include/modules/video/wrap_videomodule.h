#pragma once

#include "common/luax.h"
#include "modules/video/videomodule.h"

namespace Wrap_VideoModule
{
    int NewVideoStream(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_VideoModule
