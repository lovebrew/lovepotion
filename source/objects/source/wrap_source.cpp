#include <objects/source/wrap_source.hpp>

using namespace love;
using Source = love::Source<Console::Which>;

::Source* Wrap_Source::CheckSource(lua_State* L, int index)
{
    return luax::CheckType<::Source>(L, index);
}

int Wrap_Source::Clone(lua_State* L)
{
    auto* self      = Wrap_Source::CheckSource(L, 1);
    ::Source* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_Source::Play(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    luax::PushBoolean(L, self->Play());

    return 1;
}

int Wrap_Source::Stop(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);
    self->Stop();

    return 0;
}

int Wrap_Source::Pause(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);
    self->Pause();

    return 0;
}

int Wrap_Source::SetVolume(lua_State* L)
{
    auto* self   = Wrap_Source::CheckSource(L, 1);
    float volume = luaL_checknumber(L, 2);

    self->SetVolume(volume);

    return 0;
}

int Wrap_Source::SetPitch(lua_State* L)
{
    auto* self  = Wrap_Source::CheckSource(L, 1);
    float pitch = luaL_checknumber(L, 2);

    if (pitch != pitch)
        return luaL_error(L, "Pitch cannot be NaN.");

    if (pitch > std::numeric_limits<lua_Number>::max() || pitch <= 0.0f)
        return luaL_error(L, "Pitch has to be non-zero, positive, finite number.");

    // self->SetPitch(pitch);

    return 0;
}

int Wrap_Source::GetPitch(lua_State* L)
{
    return 0;
}

int Wrap_Source::GetVolume(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    lua_pushnumber(L, self->GetVolume());

    return 1;
}

int Wrap_Source::Seek(lua_State* L)
{
    auto* self    = Wrap_Source::CheckSource(L, 1);
    double offset = luaL_checknumber(L, 2);

    if (offset < 0)
        return luaL_argerror(L, 2, "Can't seek to a negative position!");

    const char* type = lua_isnoneornil(L, 3) ? 0 : lua_tostring(L, 3);

    if (auto found = ::Source::unitTypes.Find(type))
        self->Seek(offset, *found);
    else
        return luax::EnumError(L, "time unit", ::Source::unitTypes, type);

    return 0;
}

int Wrap_Source::Tell(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    const char* type = lua_isnoneornil(L, 2) ? 0 : lua_tostring(L, 2);

    if (auto found = ::Source::unitTypes.Find(type))
        lua_pushnumber(L, self->Tell(*found));
    else
        return luax::EnumError(L, "time unit", ::Source::unitTypes, type);

    return 1;
}

int Wrap_Source::GetDuration(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    const char* type = lua_isnoneornil(L, 2) ? 0 : lua_tostring(L, 2);

    if (auto found = ::Source::unitTypes.Find(type))
        lua_pushnumber(L, self->GetDuration(*found));
    else
        return luax::EnumError(L, "time unit", ::Source::unitTypes, type);

    return 1;
}

int Wrap_Source::SetLooping(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    luax::CatchException(L, [&]() { self->SetLooping(luax::CheckBoolean(L, 2)); });

    return 0;
}

int Wrap_Source::IsLooping(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    luax::PushBoolean(L, self->IsLooping());

    return 1;
}

int Wrap_Source::IsPlaying(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    luax::PushBoolean(L, self->IsPlaying());

    return 1;
}

int Wrap_Source::SetVolumeLimits(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 2);

    if (min < 0.0f || min > 1.0f || max < 0.0f || max > 1.0f)
        return luaL_error(L, "Invalid volume limits: [%f:%f]. Must be in [0:1]", min, max);

    self->SetMinVolume(min);
    self->SetMaxVolume(max);

    return 0;
}

int Wrap_Source::GetVolumeLimits(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    lua_pushnumber(L, self->GetMinVolume());
    lua_pushnumber(L, self->GetMaxVolume());

    return 2;
}

int Wrap_Source::GetChannelCount(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    lua_pushinteger(L, self->GetChannelCount());

    return 1;
}

int Wrap_Source::GetFreeBufferCount(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    lua_pushinteger(L, self->GetFreeBufferCount());

    return 1;
}

int Wrap_Source::Queue(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    bool success = false;

    if (luax::IsType(L, 2, SoundData::type))
    {
        auto* soundData = luax::ToType<SoundData>(L, 2);

        int offset    = 0;
        size_t length = soundData->GetSize();

        if (lua_gettop(L) == 4)
        {
            offset = luaL_checknumber(L, 3);
            length = luaL_checknumber(L, 4);
        }
        else if (lua_gettop(L) == 3)
            length = luaL_checknumber(L, 3);

        if (offset < 0 || length > soundData->GetSize() - offset)
            return luaL_error(L, "Data region is out of bounds!");

        luax::CatchException(L, [&]() {
            success = self->Queue(soundData->GetData() + offset, length, soundData->GetSampleRate(),
                                  soundData->GetBitDepth(), soundData->GetChannelCount());
        });
    }
    else if (lua_islightuserdata(L, 2))
    {
        int offset     = luaL_checknumber(L, 3);
        int length     = luaL_checknumber(L, 4);
        int sampleRate = luaL_checknumber(L, 5);
        int bitDepth   = luaL_checknumber(L, 6);
        int channels   = luaL_checknumber(L, 7);

        if (length < 0 || offset < 0)
            return luaL_error(L, "Data region is out of bounds!");

        luax::CatchException(L, [&]() {
            success = self->Queue((void*)((uintptr_t)lua_touserdata(L, 2) + (uintptr_t)offset),
                                  length, sampleRate, bitDepth, channels);
        });
    }
    else
        return luax::TypeError(L, 2, "SoundData or lightuserdata");

    luax::PushBoolean(L, success);

    return 1;
}

int Wrap_Source::GetType(lua_State* L)
{
    auto* self = Wrap_Source::CheckSource(L, 1);

    auto type = self->GetType();

    if (auto found = ::Source::sourceTypes.ReverseFind(type))
        lua_pushstring(L, *found);
    else
        return luaL_error(L, "Unknown Source type!");

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",              Wrap_Source::Clone              },
    { "play",               Wrap_Source::Play               },
    { "pause",              Wrap_Source::Pause              },
    { "stop",               Wrap_Source::Stop               },
    { "setVolume",          Wrap_Source::SetVolume          },
    { "getVolume",          Wrap_Source::GetVolume          },
    { "seek",               Wrap_Source::Seek               },
    { "tell",               Wrap_Source::Tell               },
    { "getDuration",        Wrap_Source::GetDuration        },
    { "setLooping",         Wrap_Source::SetLooping         },
    { "isLooping",          Wrap_Source::IsLooping          },
    { "isPlaying",          Wrap_Source::IsPlaying          },
    { "setVolumeLimits",    Wrap_Source::SetVolumeLimits    },
    { "getVolumeLimits",    Wrap_Source::GetVolumeLimits    },
    { "getChannelCount",    Wrap_Source::GetChannelCount    },
    { "getFreeBufferCount", Wrap_Source::GetFreeBufferCount },
    { "queue",              Wrap_Source::Queue              },
    { "getType",            Wrap_Source::GetType            }
};
// clang-format on

int Wrap_Source::Register(lua_State* L)
{
    return luax::RegisterType(L, &::Source::type, functions);
}
