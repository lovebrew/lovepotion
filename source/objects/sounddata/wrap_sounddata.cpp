#include "common/runtime.h"
#include "objects/sounddata/wrap_sounddata.h"

using namespace love;

int Wrap_SoundData::Clone(lua_State * L)
{
    SoundData * self = Wrap_SoundData::CheckSoundData(L, 1);
    SoundData * clone = nullptr;

    Luax::CatchException(L, [&]() {
        clone = self->Clone();
    });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_SoundData::GetBitDepth(lua_State * L)
{
    SoundData * self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetBitDepth());

    return 1;
}

int Wrap_SoundData::GetChannelCount(lua_State * L)
{
    SoundData * self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetChannelCount());

    return 1;
}

int Wrap_SoundData::GetDuration(lua_State * L)
{
    SoundData * self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushnumber(L, self->GetDuration());

    return 1;
}

int Wrap_SoundData::GetSampleCount(lua_State * L)
{
    SoundData * self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetSampleCount());

    return 1;
}

int Wrap_SoundData::GetSampleRate(lua_State * L)
{
    SoundData * self = Wrap_SoundData::CheckSoundData(L, 1);

    lua_pushinteger(L, self->GetSampleRate());

    return 1;
}

SoundData * Wrap_SoundData::CheckSoundData(lua_State * L, int index)
{
    return Luax::CheckType<SoundData>(L, index);
}

int Wrap_SoundData::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "clone",           Clone           },
        { "getBitDepth",     GetBitDepth     },
        { "getChannelCount", GetChannelCount },
        { "getDuration",     GetDuration     },
        { "getSampleCount",  GetSampleCount  },
        { "getSampleRate",   GetSampleRate   },
        { 0, 0 }
    };

    return Luax::RegisterType(L, &SoundData::type, reg, nullptr);
}
