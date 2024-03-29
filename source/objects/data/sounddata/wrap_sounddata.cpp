#include <objects/data/sounddata/wrap_sounddata.hpp>

#include <objects/data/wrap_data.hpp>

using namespace love;

int Wrap_SoundData::Clone(lua_State* L)
{
    SoundData* self  = Wrap_SoundData::CheckSoundData(L, 1);
    SoundData* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_SoundData::GetBitDepth(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetBitDepth());

    return 1;
}

int Wrap_SoundData::GetChannelCount(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetChannelCount());

    return 1;
}

int Wrap_SoundData::GetDuration(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushnumber(L, self->GetDuration());

    return 1;
}

int Wrap_SoundData::GetSampleCount(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetSampleCount());

    return 1;
}

int Wrap_SoundData::GetSampleRate(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetSampleRate());

    return 1;
}

int Wrap_SoundData::SetSample(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);

    int i = (int)luaL_checkinteger(L, 2);

    if (lua_gettop(L) > 3)
    {
        int channel  = luaL_checkinteger(L, 3);
        float sample = (float)luaL_checknumber(L, 4);

        luax::CatchException(L, [&]() { self->SetSample(i, channel, sample); });
    }
    else
    {
        float sample = (float)luaL_checknumber(L, 3);

        luax::CatchException(L, [&]() { self->SetSample(i, sample); });
    }

    return 0;
}

int Wrap_SoundData::GetSample(lua_State* L)
{
    SoundData* self = Wrap_SoundData::CheckSoundData(L, 1);
    int i           = (int)luaL_checkinteger(L, 2);

    if (lua_gettop(L) > 2)
    {
        int channel = luaL_checkinteger(L, 3);

        luax::CatchException(L, [&]() { lua_pushnumber(L, self->GetSample(i, channel)); });
    }
    else
        luax::CatchException(L, [&]() { self->GetSample(i); });

    return 1;
}

SoundData* Wrap_SoundData::CheckSoundData(lua_State* L, int index)
{
    return luax::CheckType<SoundData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",           Wrap_SoundData::Clone           },
    { "getBitDepth",     Wrap_SoundData::GetBitDepth     },
    { "getChannelCount", Wrap_SoundData::GetChannelCount },
    { "getDuration",     Wrap_SoundData::GetDuration     },
    { "getSample",       Wrap_SoundData::GetSample       },
    { "getSampleCount",  Wrap_SoundData::GetSampleCount  },
    { "getSampleRate",   Wrap_SoundData::GetSampleRate   },
    { "setSample",       Wrap_SoundData::SetSample       }
};
// clang-format on

int Wrap_SoundData::Register(lua_State* L)
{
    return luax::RegisterType(L, &SoundData::type, Wrap_Data::functions, functions);
}
