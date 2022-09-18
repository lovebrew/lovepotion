#include <objects/data/sounddata/sounddata.hpp>
#include <objects/data/sounddata/wrap_sounddata.hpp>

#include <objects/decoder/wrap_decoder.hpp>
#include <objects/file/file.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>
#include <modules/sound/sound.hpp>

#include <utilities/stream/types/datastream.hpp>

#include <modules/sound/wrap_sound.hpp>

using namespace love;

#define instance() (Module::GetInstance<Sound>(Module::M_SOUND))

int Wrap_Sound::NewDecoder(lua_State* L)
{
    int bufferSize = luaL_optinteger(L, 2, Decoder::DEFAULT_BUFFER_SIZE);
    Stream* stream = nullptr;

    if (Wrap_Filesystem::CanGetFile(L, 1))
    {
        auto sourceType    = Decoder::STREAM_FILE;
        const char* source = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);

        if (source != nullptr && !Decoder::GetConstant(source, sourceType))
            return luax::EnumError(L, "stream type", Decoder::GetConstants(sourceType), source);

        if (sourceType == Decoder::STREAM_FILE)
        {
            auto file = Wrap_Filesystem::GetFile(L, 1);

            luax::CatchException(L, [&]() { file->Open(File::MODE_READ); });
            stream = file;
        }
        else
        {
            luax::CatchException(L, [&]() {
                StrongReference<FileData> data(Wrap_Filesystem::GetFileData(L, 1, true),
                                               Acquire::NORETAIN);

                stream = new DataStream(data);
            });
        }
    }
    else if (luax::IsType(L, 1, Data::type))
    {
        Data* data = luax::CheckType<Data>(L, 1);
        luax::CatchException(L, [&]() { stream = new DataStream(data); });
    }
    else
    {
        stream = luax::CheckType<Stream>(L, 1);
        stream->Retain();
    }

    Decoder* self = nullptr;

    luax::CatchException(
        L, [&]() { self = instance()->NewDecoder(stream, bufferSize); },
        [&](bool) { stream->Release(); });

    luax::PushType(L, self);
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

        luax::CatchException(
            L, [&]() { data = instance()->NewSoundData(samples, sampleRate, bitDepth, channels); });
    }
    else
    {
        if (!luax::IsType(L, 1, Decoder::type))
        {
            Wrap_Sound::NewDecoder(L);
            lua_replace(L, 1);
        }

        luax::CatchException(
            L, [&]() { data = instance()->NewSoundData(Wrap_Decoder::CheckDecoder(L, 1)); });
    }

    luax::PushType(L, data);
    data->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newDecoder",   Wrap_Sound::NewDecoder   },
    { "newSoundData", Wrap_Sound::NewSoundData }
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
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Sound(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "sound";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
