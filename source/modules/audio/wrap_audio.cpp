#include "modules/audio/wrap_audio.h"

using namespace love;

#define instance() (Module::GetInstance<Audio>(Module::M_AUDIO))

int Wrap_Audio::GetVolume(lua_State * L)
{
    lua_pushnumber(L, instance()->GetVolume());

    return 1;
}

int Wrap_Audio::NewSource(lua_State * L)
{
    Source::Type type = Source::TYPE_STREAM;

    if (!Luax::IsType(L, 1, SoundData::type) && !Luax::IsType(L, 1, Decoder::type))
    {
        const char * typeString = luaL_checkstring(L, 2);

        if (typeString && !Source::GetConstant(typeString, type))
            return Luax::EnumError(L, "source type", Source::GetConstants(type), typeString);

        if (type == Source::TYPE_QUEUE)
            return luaL_error(L, "Cannot create queueable sources using newSource. Use newQueueableSource instead.");
    }

    if (lua_isstring(L, 1) || Luax::IsType(L, 1, File::type) || Luax::IsType(L, 1, FileData::type))
        Luax::ConvertObject(L, 1, "sound", "newDecoder");

    if (type == Source::TYPE_STATIC && Luax::IsType(L, 1, Decoder::type))
        Luax::ConvertObject(L, 1, "sound", "newSoundData");

    Source * source = nullptr;

    Luax::CatchException(L, [&]() {
        if (Luax::IsType(L, 1, SoundData::type))
            source = instance()->NewSource(Luax::ToType<SoundData>(L, 1));
        else if (Luax::IsType(L, 1, Decoder::type))
            source = instance()->NewSource(Luax::ToType<Decoder>(L, 1));
    });

    if (source != nullptr)
    {
        Luax::PushType(L, source);
        source->Release();

        return 1;
    }
    else
        return Luax::TypeErrror(L, 1, "Decoder or SoundData");
}

static std::vector<Source *> ReadSourceList(lua_State * L, int index)
{
    if (index < 0)
        index += lua_gettop(L) + 1;

    size_t numItems = lua_objlen(L, index);
    std::vector<Source *> sources(numItems);

    for (size_t i = 0; i < numItems; i++)
    {
        lua_rawgeti(L, index, i + 1);
        sources[i] = Wrap_Source::CheckSource(L, -1);
        lua_pop(L, 1);
    }

    return sources;
}

static std::vector<Source *> ReadSourceVaArg(lua_State * L, int index)
{
    const int top = lua_gettop(L);

    if (index < 0)
        index += top + 1;

    size_t numItems = top - 1 + 1;
    std::vector<Source *> sources(numItems);

    for (int position = 0; position <= top; index++, position++)
        sources[position] = Wrap_Source::CheckSource(L, index);

    return sources;
}

int Wrap_Audio::Play(lua_State * L)
{
    Source * source = Wrap_Source::CheckSource(L, 1);

    lua_pushboolean(L, instance()->Play(source));

    return 1;
}

int Wrap_Audio::Pause(lua_State * L)
{
    if (lua_isnone(L, 1))
        printf("Pause all Sources");
    else
    {
        Source * source = Wrap_Source::CheckSource(L, 1);
        source->Pause();
    }

    return 0;
}

int Wrap_Audio::SetVolume(lua_State * L)
{
    float volume = (float)luaL_checknumber(L, 1);
    instance()->SetVolume(volume);

    return 0;
}

int Wrap_Audio::Stop(lua_State * L)
{
    if (lua_isnone(L, 1))
        instance()->Stop();
    else
    {
        Source * source = Wrap_Source::CheckSource(L, 1);
        source->Stop();
    }

    return 0;
}

int Wrap_Audio::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "getVolume",     GetVolume    },
        { "newSource",     NewSource    },
        { "pause",         Pause        },
        { "play",          Play         },
        { "setVolume",     SetVolume    },
        { "stop",          Stop         },
        { 0,               0            }
    };

    lua_CFunction types[] =
    {
        Wrap_Source::Register,
        0
    };

    Audio * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Audio(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance = instance;
    wrappedModule.name = "audio";
    wrappedModule.type = &Module::type;
    wrappedModule.functions = reg;
    wrappedModule.types = types;

    return Luax::RegisterModule(L, wrappedModule);
}
