#include "modules/sound/wrap_SoundData.hpp"
#include "modules/data/wrap_Data.hpp"

using namespace love;

int Wrap_SoundData::clone(lua_State* L)
{
    auto* self       = luax_checksounddata(L, 1);
    SoundData* clone = nullptr;

    luax_catchexcept(L, [&]() { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_SoundData::getChannelCount(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);

    lua_pushinteger(L, self->getChannelCount());

    return 1;
}

int Wrap_SoundData::getBitDepth(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);

    lua_pushinteger(L, self->getBitDepth());

    return 1;
}

int Wrap_SoundData::getSampleRate(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);

    lua_pushinteger(L, self->getSampleRate());

    return 1;
}

int Wrap_SoundData::getSampleCount(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);

    lua_pushinteger(L, self->getSampleCount());

    return 1;
}

int Wrap_SoundData::getDuration(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);

    lua_pushnumber(L, self->getDuration());

    return 1;
}

int Wrap_SoundData::setSample(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);
    int i      = (int)luaL_checkinteger(L, 2);

    if (lua_gettop(L) > 3)
    {
        int channel  = luaL_checkinteger(L, 3);
        float sample = luaL_checknumber(L, 4);

        luax_catchexcept(L, [&]() { self->setSample(i, channel, sample); });
    }
    else
    {
        float sample = luaL_checknumber(L, 3);
        luax_catchexcept(L, [&]() { self->setSample(i, sample); });
    }

    return 0;
}

int Wrap_SoundData::getSample(lua_State* L)
{
    auto* self = luax_checksounddata(L, 1);
    int i      = (int)luaL_checkinteger(L, 2);

    if (lua_gettop(L) > 2)
    {
        int channel = luaL_checkinteger(L, 3);

        luax_catchexcept(L, [&]() { lua_pushnumber(L, self->getSample(i, channel)); });
    }
    else
        luax_catchexcept(L, [&]() { lua_pushnumber(L, self->getSample(i)); });

    return 1;
}

int Wrap_SoundData::copyFrom(lua_State* L)
{
    auto* destination  = luax_checksounddata(L, 1);
    const auto* source = luax_checksounddata(L, 2);

    int sourceStart = luaL_checkinteger(L, 3);
    int count       = luaL_checkinteger(L, 4);
    int dstStart    = luaL_optinteger(L, 5, 0);

    luax_catchexcept(L, [&]() { destination->copyFrom(source, sourceStart, count, dstStart); });

    return 0;
}

int Wrap_SoundData::slice(lua_State* L)
{
    auto* self       = luax_checksounddata(L, 1);
    SoundData* slice = nullptr;

    int start  = luaL_checkinteger(L, 2);
    int length = luaL_optinteger(L, 3, -1);

    luax_catchexcept(L, [&]() { slice = self->slice(start, length); });

    luax_pushtype(L, slice);
    slice->release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",            Wrap_SoundData::clone           },
    { "getChannelCount",  Wrap_SoundData::getChannelCount },
    { "getBitDepth",      Wrap_SoundData::getBitDepth     },
    { "getSampleRate",    Wrap_SoundData::getSampleRate   },
    { "getSampleCount",   Wrap_SoundData::getSampleCount  },
    { "getDuration",      Wrap_SoundData::getDuration     },
    { "setSample",        Wrap_SoundData::setSample       },
    { "getSample",        Wrap_SoundData::getSample       },
    { "copyFrom",         Wrap_SoundData::copyFrom        },
    { "slice",            Wrap_SoundData::slice           }
};
// clang-format on

namespace love
{
    SoundData* luax_checksounddata(lua_State* L, int index)
    {
        return luax_checktype<SoundData>(L, index);
    }

    int open_sounddata(lua_State* L)
    {
        return luax_register_type(L, &SoundData::type, functions, Wrap_Data::functions);
    }
} // namespace love
