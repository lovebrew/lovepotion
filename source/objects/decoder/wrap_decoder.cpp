#include "objects/decoder/wrap_decoder.h"
#include "modules/sound/sound.h"

using namespace love;

#define instance() (Module::GetInstance<Sound>(Module::M_SOUND))

int Wrap_Decoder::Clone(lua_State* L)
{
    Decoder* self  = Wrap_Decoder::CheckDecoder(L, 1);
    Decoder* clone = nullptr;

    Luax::CatchException(L, [&]() { clone = self->Clone(); });

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_Decoder::GetChannelCount(lua_State* L)
{
    Decoder* self = Wrap_Decoder::CheckDecoder(L, 1);

    lua_pushinteger(L, self->GetChannelCount());

    return 1;
}

int Wrap_Decoder::GetBitDepth(lua_State* L)
{
    Decoder* self = Wrap_Decoder::CheckDecoder(L, 1);

    lua_pushinteger(L, self->GetBitDepth());

    return 1;
}

int Wrap_Decoder::GetSampleRate(lua_State* L)
{
    Decoder* self = Wrap_Decoder::CheckDecoder(L, 1);

    lua_pushinteger(L, self->GetSampleRate());

    return 1;
}

int Wrap_Decoder::GetDuration(lua_State* L)
{
    Decoder* self = Wrap_Decoder::CheckDecoder(L, 1);

    lua_pushnumber(L, self->GetDuration());

    return 1;
}

int Wrap_Decoder::Decode(lua_State* L)
{
    Decoder* self = Wrap_Decoder::CheckDecoder(L, 1);
    int decoded   = self->Decode();

    if (decoded > 0)
    {
        Luax::CatchException(L, [&]() {
            SoundData* soundData = instance()->NewSoundData(
                self->GetBuffer(), decoded / (self->GetBitDepth() / 8 * self->GetChannelCount()),
                self->GetSampleRate(), self->GetBitDepth(), self->GetChannelCount());

            Luax::PushType(L, soundData);
            soundData->Release();
        });
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Decoder::Seek(lua_State* L)
{
    Decoder* self = Wrap_Decoder::CheckDecoder(L, 1);
    double offset = luaL_checknumber(L, 2);

    if (offset < 0)
        return luaL_argerror(L, 2, "cannot seek to a negative position!");
    else if (offset == 0)
        self->Rewind();
    else
        self->Seek(offset);

    return 0;
}

Decoder* Wrap_Decoder::CheckDecoder(lua_State* L, int index)
{
    return Luax::CheckType<Decoder>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",           Wrap_Decoder::Clone           },
    { "decode",          Wrap_Decoder::Decode          },
    { "getBitDepth",     Wrap_Decoder::GetBitDepth     },
    { "getChannelCount", Wrap_Decoder::GetChannelCount },
    { "getDuration",     Wrap_Decoder::GetDuration     },
    { "getSampleRate",   Wrap_Decoder::GetSampleRate   },
    { "seek",            Wrap_Decoder::Seek            },
    { 0,                 0                             }
};
// clang-format on

int Wrap_Decoder::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Decoder::type, functions, nullptr);
}
