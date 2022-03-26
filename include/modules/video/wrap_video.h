#pragma once

#include "common/luax.h"
#include "modules/video/video.h"

namespace Wrap_VideoModule
{
    int NewVideo(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_VideoModule
