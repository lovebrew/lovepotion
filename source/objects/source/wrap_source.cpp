#include "objects/source/wrap_source.h"

using namespace love;

int Wrap_Source::Clone(lua_State* L)
{
    Source* self  = Wrap_Source::CheckSource(L, 1);
    Source* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = self->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_Source::GetChannelCount(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushinteger(L, self->GetChannelCount());

    return 1;
}

int Wrap_Source::GetFreeBufferCount(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushinteger(L, self->GetFreeBufferCount());

    return 1;
}

int Wrap_Source::GetType(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    Source::Type type  = self->GetType();
    const char* string = 0;

    if (!Source::GetConstant(type, string))
        return luaL_error(L, "Unknown Source type.");

    lua_pushstring(L, string);

    return 1;
}

int Wrap_Source::GetVolume(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushnumber(L, self->GetVolume());

    return 1;
}

int Wrap_Source::GetVolumeLimits(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushnumber(L, self->GetMinVolume());
    lua_pushnumber(L, self->GetMaxVolume());

    return 2;
}

int Wrap_Source::IsLooping(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, self->IsLooping());

    return 1;
}

int Wrap_Source::IsPlaying(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, self->IsPlaying());

    return 1;
}

int Wrap_Source::Pause(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    self->Pause();

    return 0;
}

int Wrap_Source::Play(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, self->Play());

    return 1;
}

int Wrap_Source::Seek(lua_State* L)
{
    Source* self  = Wrap_Source::CheckSource(L, 1);
    double offset = luaL_checknumber(L, 2);

    if (offset < 0)
        return luaL_argerror(L, 2, "can't seek to a negative position.");

    Source::Unit unit      = Source::UNIT_SECONDS;
    const char* unitString = lua_isnoneornil(L, 3) ? 0 : lua_tostring(L, 3);

    if (unitString && !self->GetConstant(unitString, unit))
        return Luax::EnumError(L, "time unit", Source::GetConstants(unit), unitString);

    self->Seek(offset, unit);

    return 0;
}

int Wrap_Source::SetLooping(lua_State* L)
{
    Source* self    = Wrap_Source::CheckSource(L, 1);
    bool shouldLoop = lua_toboolean(L, 2);

    self->SetLooping(shouldLoop);

    return 0;
}

int Wrap_Source::SetVolume(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);
    float volume = luaL_checknumber(L, 2);

    self->SetVolume(volume);

    return 0;
}

int Wrap_Source::SetVolumeLimits(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    if (min < .0f || min > 1.f || max < .0f || max > 1.f)
        return luaL_error(L, "Invalid volume limits: [%f:%f]. Must be in [0:1].", min, max);

    self->SetMinVolume(min);
    self->SetMaxVolume(max);

    return 0;
}

int Wrap_Source::Stop(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    self->Stop();

    return 0;
}

int Wrap_Source::GetDuration(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    Source::Unit units = Source::UNIT_SECONDS;
    const char* unit   = lua_isnoneornil(L, 2) ? 0 : lua_tostring(L, 2);

    if (unit && !self->GetConstant(unit, units))
        return Luax::EnumError(L, "time unit", Source::GetConstants(units), unit);

    lua_pushnumber(L, self->GetDuration(units));

    return 1;
}

int Wrap_Source::Tell(lua_State* L)
{
    Source* self = Wrap_Source::CheckSource(L, 1);

    Source::Unit units = Source::UNIT_SECONDS;
    const char* unit   = lua_isnoneornil(L, 2) ? 0 : lua_tostring(L, 2);

    if (unit && !self->GetConstant(unit, units))
        return Luax::EnumError(L, "time unit", Source::GetConstants(units), unit);

    lua_pushnumber(L, self->Tell(units));

    return 1;
}

Source* Wrap_Source::CheckSource(lua_State* L, int index)
{
    return Luax::CheckType<Source>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",              Wrap_Source::Clone              },
    { "getChannelCount",    Wrap_Source::GetChannelCount    },
    { "getDuration",        Wrap_Source::GetDuration        },
    { "getFreeBufferCount", Wrap_Source::GetFreeBufferCount },
    { "getType",            Wrap_Source::GetType            },
    { "getVolume",          Wrap_Source::GetVolume          },
    { "getVolumeLimits",    Wrap_Source::GetVolumeLimits    },
    { "isLooping",          Wrap_Source::IsLooping          },
    { "isPlaying",          Wrap_Source::IsPlaying          },
    { "pause",              Wrap_Source::Pause              },
    { "play",               Wrap_Source::Play               },
    { "seek",               Wrap_Source::Seek               },
    { "setLooping",         Wrap_Source::SetLooping         },
    { "setVolume",          Wrap_Source::SetVolume          },
    { "setVolumeLimits",    Wrap_Source::SetVolumeLimits    },
    { "stop",               Wrap_Source::Stop               },
    { "tell",               Wrap_Source::Tell               },
    { 0,                    0                               }
};
// clang-format on

int Wrap_Source::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Source::type, functions, nullptr);
}
