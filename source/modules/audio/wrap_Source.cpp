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

int Wrap_Source::setLooping(lua_State* L)
{
    auto* self   = luax_checksource(L, 1);
    bool looping = luax_checkboolean(L, 2);

    self->setLooping(looping);

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",      Wrap_Source::clone      },
    { "play",       Wrap_Source::play       },
    { "pause",      Wrap_Source::pause      },
    { "stop",       Wrap_Source::stop       },
    { "setLooping", Wrap_Source::setLooping }
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
