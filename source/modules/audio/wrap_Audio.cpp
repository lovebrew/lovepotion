#include "modules/audio/wrap_Audio.hpp"
#include "modules/audio/wrap_RecordingDevice.hpp"
#include "modules/audio/wrap_Source.hpp"

#include "modules/filesystem/wrap_Filesystem.hpp"

#include "modules/audio/dsp/Audio.hpp"
#include "modules/audio/null/Audio.hpp"

using namespace love;

#define instance() Module::getInstance<AudioBase>(Module::M_AUDIO)

int Wrap_Audio::newSource(lua_State* L)
{
    auto type = SourceBase::TYPE_STREAM;

    if (!luax_istype(L, 1, SoundData::type))
    {
        if (!luax_istype(L, 1, Decoder::type))
        {
            const char* typeName = luaL_checkstring(L, 2);
            if (typeName && !SourceBase::getConstant(typeName, type))
                return luax_enumerror(L, "source type", SourceBase::SourceTypes, typeName);

            if (type == SourceBase::TYPE_QUEUE)
                return luaL_error(L, E_CANNOT_CREATE_QUEUE_SOURCE);
        }

        if (luax_cangetdata(L, 1))
        {
            if (type == SourceBase::TYPE_STATIC)
                lua_pushstring(L, "memory");
            else if (!lua_isnone(L, 3))
                lua_pushvalue(L, 3);
            else
                lua_pushnil(L);

            lua_pushnil(L);

            std::array<int, 3> indexes { 1, lua_gettop(L), lua_gettop(L) - 1 };
            luax_convobj(L, indexes, "sound", "newDecoder");
        }
    }

    if (type == SourceBase::TYPE_STATIC && luax_istype(L, 1, Decoder::type))
        luax_convobj(L, 1, "sound", "newSoundData");

    SourceBase* source = nullptr;

    luax_catchexcept(L, [&] {
        if (luax_istype(L, 1, SoundData::type))
            source = instance()->newSource(luax_totype<SoundData>(L, 1));
        else if (luax_istype(L, 1, Decoder::type))
            source = instance()->newSource(luax_totype<Decoder>(L, 1));
    });

    if (source != nullptr)
    {
        luax_pushtype(L, source);
        source->release();

        return 1;
    }
    else
        return luax_typeerror(L, 1, "Decoder or SoundData");
}

static std::vector<SourceBase*> readSourceList(lua_State* L, int index)
{
    if (index < 0)
        index += lua_gettop(L) + 1;

    int count = luax_objlen(L, index);
    std::vector<SourceBase*> sources(count);

    for (int i = 0; i < count; i++)
    {
        lua_rawgeti(L, index, i + 1);
        sources[i] = luax_checksource(L, -1);
        lua_pop(L, 1);
    }

    return sources;
}

static std::vector<SourceBase*> readSourceVararg(lua_State* L, int index)
{
    const auto top = lua_gettop(L);

    if (index < 0)
        index += top + 1;

    int count = top - index + 1;
    std::vector<SourceBase*> sources(count);

    for (int i = 0; i <= top; index++, i++)
        sources[i] = luax_checksource(L, -1);

    return sources;
}

int Wrap_Audio::play(lua_State* L)
{
    if (lua_istable(L, 1))
        luax_pushboolean(L, instance()->play(readSourceList(L, 1)));
    else if (lua_gettop(L) > 1)
        luax_pushboolean(L, instance()->play(readSourceVararg(L, 1)));
    else
    {
        auto* source = luax_checksource(L, 1);
        luax_pushboolean(L, source->play());
    }

    return 1;
}

int Wrap_Audio::stop(lua_State* L)
{
    if (lua_isnone(L, 1))
        instance()->stop();
    else if (lua_istable(L, 1))
        instance()->stop(readSourceList(L, 1));
    else if (lua_gettop(L) > 1)
        instance()->stop(readSourceVararg(L, 1));
    else
    {
        auto* source = luax_checksource(L, 1);
        source->stop();
    }

    return 0;
}

int Wrap_Audio::pause(lua_State* L)
{
    if (lua_isnone(L, 1))
    {
        auto sources = instance()->pause();
        lua_createtable(L, (int)sources.size(), 0);

        for (int index = 0; index < (int)sources.size(); index++)
        {
            luax_pushtype(L, sources[index]);
            lua_rawseti(L, -2, index + 1);
        }

        return 1;
    }
    else if (lua_istable(L, 1))
        instance()->pause(readSourceList(L, 1));
    else if (lua_gettop(L) > 1)
        instance()->pause(readSourceVararg(L, 1));
    else
    {
        auto* source = luax_checksource(L, 1);
        source->pause();
    }

    return 0;
}

int Wrap_Audio::setVolume(lua_State* L)
{
    float volume = luaL_checknumber(L, 1);
    instance()->setVolume(volume);

    return 0;
}

int Wrap_Audio::getVolume(lua_State* L)
{
    lua_pushnumber(L, instance()->getVolume());

    return 1;
}

int Wrap_Audio::getRecordingDevices(lua_State* L)
{
    const auto& devices = instance()->getRecordingDevices();

    lua_createtable(L, (int)devices.size(), 0);
    for (int index = 0; index < (int)devices.size(); index++)
    {
        luax_pushtype(L, devices[index]);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_Audio::setMixWithSystem(lua_State* L)
{
    bool mix = luax_checkboolean(L, 1);
    luax_pushboolean(L, AudioBase::setMixWithSystem(mix));

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newSource",           Wrap_Audio::newSource           },
    { "play",                Wrap_Audio::play                },
    { "stop",                Wrap_Audio::stop                },
    { "pause",               Wrap_Audio::pause               },
    { "setVolume",           Wrap_Audio::setVolume           },
    { "getVolume",           Wrap_Audio::getVolume           },
    { "getRecordingDevices", Wrap_Audio::getRecordingDevices },
    { "setMixWithSystem",    Wrap_Audio::setMixWithSystem    }
};

static constexpr lua_CFunction types[] =
{
    love::open_source,
    love::open_recordingdevice
};
// clang-format on

int Wrap_Audio::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        try
        {
            instance = new love::audio::dsp::Audio();
        }
        catch (love::Exception& e)
        {
            std::printf("%s\n", e.what());
        }
    else
        instance->retain();

    if (instance == nullptr)
    {
        try
        {
            instance = new love::audio::null::Audio();
        }
        catch (love::Exception& e)
        {
            std::printf("%s\n", e.what());
        }
    }

    if (instance == nullptr)
        return luaL_error(L, "Could not open any audio module.");

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "audio";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
