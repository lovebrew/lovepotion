#pragma once

#include "common/luax.hpp"
#include "modules/video/VideoStream.hpp"

namespace Wrap_Video
{
    int newVideoStream(lua_State* L);

    int open(lua_State* L);
} // namespace Wrap_Video
