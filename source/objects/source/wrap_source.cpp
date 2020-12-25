#include "common/luax.h"
#include "objects/source/wrap_source.h"

using namespace love;

int Wrap_Source::Clone(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);
    Source * clone = nullptr;

    Luax::CatchException(L, [&]() {
        clone = self->Clone();
    });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_Source::GetChannelCount(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushinteger(L, self->GetChannelCount());

    return 1;
}

int Wrap_Source::GetDuration(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    Source::Unit unit = Source::UNIT_SECONDS;
    const char * unitString = lua_isnoneornil(L, 2) ? 0 : lua_tostring(L, 2);

    if (unitString && !self->GetConstant(unitString, unit))
        return Luax::EnumError(L, "time unit", Source::GetConstants(unit), unitString);

    lua_pushnumber(L, self->GetDuration(unit));

    return 1;
}

int Wrap_Source::GetFreeBufferCount(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushinteger(L, self->GetFreeBufferCount());

    return 1;
}

int Wrap_Source::GetType(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    Source::Type type = self->GetType();
    const char * string = 0;

    if (!Source::GetConstant(type, string))
        return luaL_error(L, "Unknown Source type.");

    lua_pushstring(L, string);

    return 1;
}

int Wrap_Source::GetVolume(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushnumber(L, self->GetVolume());

    return 1;
}

int Wrap_Source::GetVolumeLimits(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushnumber(L, self->GetMinVolume());
    lua_pushnumber(L, self->GetMaxVolume());

    return 2;
}

int Wrap_Source::IsLooping(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, self->IsLooping());

    return 1;
}

int Wrap_Source::IsPlaying(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, self->IsPlaying());

    return 1;
}

int Wrap_Source::Pause(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    self->Pause();

    return 0;
}

int Wrap_Source::Play(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, self->Play());

    return 1;
}

int Wrap_Source::Seek(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);
    double offset = luaL_checknumber(L, 2);

    if (offset < 0)
        return luaL_argerror(L, 2, "can't seek to a negative position.");

    Source::Unit unit = Source::UNIT_SECONDS;
    const char * unitString = lua_isnoneornil(L, 3) ? 0 : lua_tostring(L, 3);

    if (unitString && !self->GetConstant(unitString, unit))
        return Luax::EnumError(L, "time unit", Source::GetConstants(unit), unitString);

    self->Seek(offset, unit);

    return 0;
}

int Wrap_Source::SetLooping(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);
    bool shouldLoop = lua_toboolean(L, 2);

    self->SetLooping(shouldLoop);

    return 0;
}

int Wrap_Source::SetVolume(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);
    float volume = luaL_checknumber(L, 2);

    self->SetVolume(volume);

    return 0;
}

int Wrap_Source::SetVolumeLimits(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    if (min < .0f || min > 1.f || max < .0f || max > 1.f)
        return luaL_error(L, "Invalid volume limits: [%f:%f]. Must be in [0:1].", min, max);

    self->SetMinVolume(min);
    self->SetMaxVolume(max);

    return 0;
}

int Wrap_Source::Stop(lua_State * L)
{
    Source * self = Wrap_Source::CheckSource(L, 1);

    self->Stop();

    return 0;
}

Source * Wrap_Source::CheckSource(lua_State * L, int index)
{
    return Luax::CheckType<Source>(L, index);
}

int Wrap_Source::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "clone",              Clone              },
        { "getChannelCount",    GetChannelCount    },
        { "getFreeBufferCount", GetFreeBufferCount },
        { "getType",            GetType            },
        { "getVolume",          GetVolume          },
        { "getVolumeLimits",    GetVolumeLimits    },
        { "isLooping",          IsLooping          },
        { "isPlaying",          IsPlaying          },
        { "pause",              Pause              },
        { "play",               Play               },
        { "seek",               Seek               },
        { "setLooping",         SetLooping         },
        { "setVolume",          SetVolume          },
        { "setVolumeLimits",    SetVolumeLimits    },
        { "stop",               Stop               },
        { 0, 0 }
    };

    return Luax::RegisterType(L, &Source::type, reg, nullptr);
}
