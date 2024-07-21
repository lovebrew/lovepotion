#include "modules/audio/wrap_Source.hpp"

using namespace love;

int Wrap_Source::clone(lua_State* L)
{
    auto* self    = luax_checksource(L, 1);
    Source* clone = nullptr;

    luax_catchexcept(L, [&] { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_Source::play(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    luax_pushboolean(L, self->play());

    return 1;
}

int Wrap_Source::stop(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    self->stop();

    return 0;
}

int Wrap_Source::pause(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    self->pause();

    return 0;
}

int Wrap_Source::setPitch(lua_State* L)
{
    auto* self  = luax_checksource(L, 1);
    float pitch = luaL_checknumber(L, 2);

    if (pitch != pitch)
        return luaL_error(L, "Pitch cannot be NaN.");

    if (pitch > std::numeric_limits<lua_Number>::max() || pitch <= 0.0f)
        return luaL_error(L, "Pitch has to be non-zero, positive, finite number.");

    // self->setPitch(pitch);

    return 0;
}

int Wrap_Source::getPitch(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushnumber(L, self->getPitch());

    return 1;
}

int Wrap_Source::setVolume(lua_State* L)
{
    auto* self   = luax_checksource(L, 1);
    float volume = luaL_checknumber(L, 2);

    self->setVolume(volume);

    return 0;
}

int Wrap_Source::getVolume(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushnumber(L, self->getVolume());

    return 1;
}

int Wrap_Source::seek(lua_State* L)
{
    auto* self    = luax_checksource(L, 1);
    double offset = luaL_checknumber(L, 2);

    if (offset < 0)
        return luaL_argerror(L, 2, "can't seek to a negative position.");

    auto type        = Source::UNIT_SECONDS;
    const char* name = lua_isnoneornil(L, 3) ? nullptr : lua_tostring(L, 3);

    if (name && !Source::getConstant(name, type))
        return luax_enumerror(L, "time unit", Source::SourceUnits, name);

    self->seek(offset, type);

    return 0;
}

int Wrap_Source::tell(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    auto type        = Source::UNIT_SECONDS;
    const char* name = lua_isnoneornil(L, 2) ? nullptr : lua_tostring(L, 2);

    if (name && !Source::getConstant(name, type))
        return luax_enumerror(L, "time unit", Source::SourceUnits, name);

    lua_pushnumber(L, self->tell(type));

    return 1;
}

int Wrap_Source::getDuration(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    auto type        = Source::UNIT_SECONDS;
    const char* name = lua_isnoneornil(L, 2) ? nullptr : lua_tostring(L, 2);

    if (name && !Source::getConstant(name, type))
        return luax_enumerror(L, "time unit", Source::SourceUnits, name);

    lua_pushnumber(L, self->getDuration(type));

    return 1;
}

int Wrap_Source::setLooping(lua_State* L)
{
    auto* self   = luax_checksource(L, 1);
    bool looping = luax_checkboolean(L, 2);

    luax_catchexcept(L, [&]() { self->setLooping(looping); });

    return 0;
}

int Wrap_Source::isLooping(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushboolean(L, self->isLooping());

    return 1;
}

int Wrap_Source::isPlaying(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushboolean(L, self->isPlaying());

    return 1;
}

int Wrap_Source::setVolumeLimits(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    if (min < 0.0f || min > 1.0f || max < 0.0f || max > 1.0f)
        return luaL_error(L, "Invalid volume limits: [%f:%f]. Must be in [0:1]", min, max);

    self->setMinVolume(min);
    self->setMaxVolume(max);

    return 0;
}

int Wrap_Source::getVolumeLimits(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushnumber(L, self->getMinVolume());
    lua_pushnumber(L, self->getMaxVolume());

    return 2;
}

int Wrap_Source::getChannelCount(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushinteger(L, self->getChannelCount());

    return 1;
}

int Wrap_Source::getFreeBufferCount(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    lua_pushinteger(L, self->getFreeBufferCount());

    return 1;
}

int Wrap_Source::getType(lua_State* L)
{
    auto* self = luax_checksource(L, 1);

    auto type = self->getType();
    std::string_view value {};

    if (!Source::getConstant(type, value))
        return luaL_error(L, "Unknown Source type.");

    luax_pushstring(L, value);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",              Wrap_Source::clone              },
    { "play",               Wrap_Source::play               },
    { "pause",              Wrap_Source::pause              },
    { "stop",               Wrap_Source::stop               },
    { "setLooping",         Wrap_Source::setLooping         },
    { "tell",               Wrap_Source::tell               },
    { "seek",               Wrap_Source::seek               },
    { "getDuration",        Wrap_Source::getDuration        },
    { "setPitch",           Wrap_Source::setPitch           },
    { "getPitch",           Wrap_Source::getPitch           },
    { "setVolume",          Wrap_Source::setVolume          },
    { "getVolume",          Wrap_Source::getVolume          },
    { "isPlaying",          Wrap_Source::isPlaying          },
    { "isLooping",          Wrap_Source::isLooping          },
    { "getVolumeLimits",    Wrap_Source::getVolumeLimits    },
    { "setVolumeLimits",    Wrap_Source::setVolumeLimits    },
    { "getChannelCount",    Wrap_Source::getChannelCount    },
    { "getFreeBufferCount", Wrap_Source::getFreeBufferCount },
    { "getType",            Wrap_Source::getType            }
};
// clang-format on

namespace love
{
    Source* luax_checksource(lua_State* L, int index)
    {
        return luax_checktype<Source>(L, index);
    }

    int open_source(lua_State* L)
    {
        return luax_register_type(L, &Source::type, functions);
    }
} // namespace love
