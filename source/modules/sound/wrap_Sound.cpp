#include "modules/sound/wrap_Sound.hpp"

#include "modules/data/DataStream.hpp"
#include "modules/filesystem/wrap_Filesystem.hpp"

#include "modules/sound/wrap_Decoder.hpp"
#include "modules/sound/wrap_SoundData.hpp"

using namespace love;

#define instance() (Module::getInstance<Sound>(Module::M_SOUND))

int Wrap_Sound::newDecoder(lua_State* L)
{
    int bufferSize = luaL_optinteger(L, 2, Decoder::DEFAULT_BUFFER_SIZE);
    Stream* stream = nullptr;

    if (luax_cangetfile(L, 1))
    {
        Decoder::StreamSource source = Decoder::STREAM_FILE;
        const char* sourceType       = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);

        if (sourceType != nullptr && !Decoder::getConstant(sourceType, source))
            return luax_enumerror(L, "stream type", Decoder::StreamSources, sourceType);

        if (source == Decoder::STREAM_FILE)
        {
            auto* file = luax_getfile(L, 1);
            luax_catchexcept(L, [&]() { file->open(File::MODE_READ); });
            stream = file;
        }
        else
        {
            luax_catchexcept(L, [&]() {
                StrongRef<FileData> data(luax_getfiledata(L, 1), Acquire::NO_RETAIN);
                stream = new DataStream(data);
            });
        }
    }
    else if (luax_istype(L, 1, Data::type))
    {
        Data* data = luax_checktype<Data>(L, 1);
        luax_catchexcept(L, [&]() { stream = new DataStream(data); });
    }
    else
    {
        stream = luax_checktype<Stream>(L, 1);
        stream->retain();
    }

    Decoder* decoder = nullptr;

    // clang-format off
    luax_catchexcept(L, [&]() {
        decoder = instance()->newDecoder(stream, bufferSize); },
        [&](bool) { stream->release(); }
    );
    // clang-format on

    luax_pushtype(L, decoder);
    decoder->release();

    return 1;
}

int Wrap_Sound::newSoundData(lua_State* L)
{
    SoundData* soundData = nullptr;

    if (lua_isnumber(L, 1))
    {
        int samples    = luaL_checkinteger(L, 1);
        int sampleRate = luaL_optinteger(L, 2, Decoder::DEFAULT_SAMPLE_RATE);
        int bitDepth   = luaL_optinteger(L, 3, Decoder::DEFAULT_BIT_DEPTH);
        int channels   = luaL_optinteger(L, 4, Decoder::DEFAULT_CHANNELS);

        luax_catchexcept(L, [&]() {
            soundData = instance()->newSoundData(samples, sampleRate, bitDepth, channels);
        });
    }
    else
    {
        if (!luax_istype(L, 1, Decoder::type))
        {
            Wrap_Sound::newDecoder(L);
            lua_replace(L, 1);
        }

        auto* decoder = luax_checkdecoder(L, 1);
        luax_catchexcept(L, [&]() { soundData = instance()->newSoundData(decoder); });
    }

    luax_pushtype(L, soundData);
    soundData->release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newDecoder",   Wrap_Sound::newDecoder   },
    { "newSoundData", Wrap_Sound::newSoundData }
};

static constexpr lua_CFunction types[] =
{
    open_sounddata,
    open_decoder
};
// clang-format on

int Wrap_Sound::open(lua_State* L)
{
    Sound* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Sound(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "sound";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
