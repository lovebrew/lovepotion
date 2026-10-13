#include "modules/video/wrap_VideoStream.hpp"
#include "common/luax.hpp"
#include "modules/thread/wrap_Channel.hpp"
#include "modules/video/VideoStream.hpp"
#include <lauxlib.h>
#include <lua.h>

using namespace love;

VideoStream* luax_checkvideostream(lua_State* L, int index)
{
    return luax_checktype<VideoStream>(L, index);
}

int Wrap_VideoStream::setSync(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);

    if (luax_istype(L, 2, SourceBase::type))
    {
        auto* source = luax_totype<SourceBase>(L, 2);
        auto* sync   = new VideoStream::SourceSync(source);
        self->setSync(sync);
        sync->release();
    }
    else if (luax_istype(L, 2, VideoStream::type))
    {
        auto* other = luax_totype<VideoStream>(L, 2);
        self->setSync(other->getSync());
    }
    else if (lua_isnoneornil(L, 2))
    {
        auto* newSync = new VideoStream::DeltaSync();
        newSync->copyState(self->getSync());
        self->setSync(newSync);
        newSync->release();
    }
    else
        return luax_typeerror(L, 2, "Source, VideoStream, or nil");

    return 0;
}

int Wrap_VideoStream::getFilename(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);
    luax_pushstring(L, self->getFilename());

    return 1;
}

int Wrap_VideoStream::play(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);
    self->play();

    return 0;
}

int Wrap_VideoStream::pause(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);
    self->pause();

    return 0;
}

int Wrap_VideoStream::seek(lua_State* L)
{
    auto* self    = luax_checkvideostream(L, 1);
    double offset = luaL_checknumber(L, 2);
    self->seek(offset);

    return 0;
}

int Wrap_VideoStream::rewind(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);
    self->seek(0);

    return 0;
}

int Wrap_VideoStream::tell(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);
    lua_pushnumber(L, self->tell());

    return 1;
}

int Wrap_VideoStream::isPlaying(lua_State* L)
{
    auto* self = luax_checkvideostream(L, 1);
    luax_pushboolean(L, self->isPlaying());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setSync",     Wrap_VideoStream::setSync     },
    { "getFilename", Wrap_VideoStream::getFilename },
    { "play",        Wrap_VideoStream::play        },
    { "pause",       Wrap_VideoStream::pause       },
    { "seek",        Wrap_VideoStream::seek        },
    { "rewind",      Wrap_VideoStream::rewind      },
    { "tell",        Wrap_VideoStream::tell        },
    { "isPlaying",   Wrap_VideoStream::isPlaying   }
};
// clang-format on

namespace love
{
    int open_videostream(lua_State* L)
    {
        return luax_register_type(L, &VideoStream::type, functions);
    }
} // namespace love
