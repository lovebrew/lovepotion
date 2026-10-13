#pragma once

#include "common/luax.hpp"
#include "modules/video/VideoStream.hpp"

namespace love
{
    int open_videostream(lua_State* L);
}

namespace Wrap_VideoStream
{
    int setSync(lua_State* L);

    int getFilename(lua_State* L);

    int play(lua_State* L);

    int pause(lua_State* L);

    int seek(lua_State* L);

    int rewind(lua_State* L);

    int tell(lua_State* L);

    int isPlaying(lua_State* L);
} // namespace Wrap_VideoStream
