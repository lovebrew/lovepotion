#pragma once

#include "common/luax.h"
#include "modules/video/video.h"

namespace Wrap_Video
{
    int NewVideoStream(lua_State* L);

    int Register(lua_State* L);
} // namespace Wrap_Video
