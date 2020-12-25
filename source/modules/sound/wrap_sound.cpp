#include "common/luax.h"
#include "modules/sound/wrap_sound.h"

using namespace love;

#define instance() (Module::GetInstance<Sound>(Module::M_SOUND))

int Wrap_Sound::NewDecoder(lua_State * L)
{
    FileData * data = Wrap_Filesystem::GetFileData(L, 1);
    int bufferSize = (int)luaL_optinteger(L, 2, Decoder::DEFAULT_BUFFER_SIZE);

    Decoder * decoder = nullptr;

    Luax::CatchException(L,
        [&]() { decoder = instance()->NewDecoder(data, bufferSize); },
        [&](bool) { data->Release(); }
    );

    if (decoder == nullptr)
        return luaL_error(L, "Extension \"%s\" not supported.", data->GetExtension().c_str());

    Luax::PushType(L, decoder);
    decoder->Release();

    return 1;
}

int Wrap_Sound::NewSoundData(lua_State * L)
{
    SoundData * data = nullptr;

    if (lua_isnumber(L, 1))
    {
        int samples    = (int)luaL_checkinteger(L, 1);
        int sampleRate = (int)luaL_optinteger(L, 2, Decoder::DEFAULT_SAMPLE_RATE);
        int bitDepth   = (int)luaL_optinteger(L, 3, Decoder::DEFAULT_BIT_DEPTH);
        int channels   = (int)luaL_optinteger(L, 4, Decoder::DEFAULT_CHANNELS);

        Luax::CatchException(L, [&]() {
            data = instance()->NewSoundData(samples, sampleRate, bitDepth, channels);
        });
    }
    else
    {
        if (!Luax::IsType(L, 1, Decoder::type))
        {
            Wrap_Sound::NewDecoder(L);
            lua_replace(L, 1);
        }

        Luax::CatchException(L, [&]() {
            data = instance()->NewSoundData(Wrap_Decoder::CheckDecoder(L, 1));
        });
    }

    Luax::PushType(L, data);
    data->Release();

    return 1;
}

int Wrap_Sound::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "newDecoder",   NewDecoder   },
        { "newSoundData", NewSoundData },
        { 0,              0            }
    };

    lua_CFunction types[] =
    {
        Wrap_SoundData::Register,
        Wrap_Decoder::Register,
        0
    };

    Sound * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Sound(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance = instance;
    wrappedModule.name = "sound";
    wrappedModule.type = &Module::type;
    wrappedModule.functions = reg;
    wrappedModule.types = types;

    return Luax::RegisterModule(L, wrappedModule);
}
