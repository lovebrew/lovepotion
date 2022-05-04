#include "modules/sound/wrap_sound.h"
#include "objects/datastream/datastream.h"

using namespace love;

#define instance() (Module::GetInstance<Sound>(Module::M_SOUND))

int Wrap_Sound::NewDecoder(lua_State* L)
{
    int bufferSize = (int)luaL_optinteger(L, 2, Decoder::DEFAULT_BUFFER_SIZE);
    Stream* stream = nullptr;

    if (Wrap_Filesystem::CanGetFile(L, 1))
    {
        Decoder::StreamSource source = Decoder::STREAM_SOURCE_FILE;
        const char* sourceType       = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);

        if (sourceType != nullptr && !Decoder::GetConstant(sourceType, source))
            return Luax::EnumError(L, "stream type", Decoder::GetConstants(source), sourceType);

        if (source == Decoder::STREAM_SOURCE_FILE)
        {
            File* file = Wrap_Filesystem::GetFile(L, 1);
            Luax::CatchException(L, [&]() { file->Open(File::MODE_READ); });

            stream = file;
        }
        else
        {
            Luax::CatchException(L, [&]() {
                StrongReference<FileData> fileData(Wrap_Filesystem::GetFileData(L, 1),
                                                   Acquire::NORETAIN);
                stream = new DataStream(fileData);
            });
        }
    }
    else if (Luax::IsType(L, 1, Data::type))
    {
        Data* data = Luax::CheckType<Data>(L, 1);
        Luax::CatchException(L, [&]() { stream = new DataStream(data); });
    }
    else
    {
        stream = Luax::CheckType<Stream>(L, 1);
        stream->Retain();
    }

    Decoder* self = nullptr;

    Luax::CatchException(
        L, [&]() { self = instance()->NewDecoder(stream, bufferSize); },
        [&](bool) { stream->Release(); });

    Luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_Sound::NewSoundData(lua_State* L)
{
    SoundData* data = nullptr;

    if (lua_isnumber(L, 1))
    {
        int samples    = (int)luaL_checkinteger(L, 1);
        int sampleRate = (int)luaL_optinteger(L, 2, Decoder::DEFAULT_SAMPLE_RATE);
        int bitDepth   = (int)luaL_optinteger(L, 3, Decoder::DEFAULT_BIT_DEPTH);
        int channels   = (int)luaL_optinteger(L, 4, Decoder::DEFAULT_CHANNELS);

        Luax::CatchException(
            L, [&]() { data = instance()->NewSoundData(samples, sampleRate, bitDepth, channels); });
    }
    else
    {
        if (!Luax::IsType(L, 1, Decoder::type))
        {
            Wrap_Sound::NewDecoder(L);
            lua_replace(L, 1);
        }

        Luax::CatchException(
            L, [&]() { data = instance()->NewSoundData(Wrap_Decoder::CheckDecoder(L, 1)); });
    }

    Luax::PushType(L, data);
    data->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newDecoder",   Wrap_Sound::NewDecoder   },
    { "newSoundData", Wrap_Sound::NewSoundData },
    { 0,              0            }
};

static constexpr lua_CFunction types[] =
{
    Wrap_SoundData::Register,
    Wrap_Decoder::Register,
    nullptr
};
// clang-format on

int Wrap_Sound::Register(lua_State* L)
{
    Sound* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Sound(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "sound";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return Luax::RegisterModule(L, wrappedModule);
}
