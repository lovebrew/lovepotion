#include "modules/sound/wrap_Decoder.hpp"

#include "modules/sound/Sound.hpp"
#include "modules/sound/SoundData.hpp"

using namespace love;

#define instance() (Module::getInstance<Sound>(Module::M_SOUND))

int Wrap_Decoder::clone(lua_State* L)
{
    auto* self     = luax_checkdecoder(L, 1);
    Decoder* clone = nullptr;

    luax_catchexcept(L, [&]() { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_Decoder::getChannelCount(lua_State* L)
{
    auto* self = luax_checkdecoder(L, 1);

    lua_pushinteger(L, self->getChannelCount());

    return 1;
}

int Wrap_Decoder::getBitDepth(lua_State* L)
{
    auto* self = luax_checkdecoder(L, 1);

    lua_pushinteger(L, self->getBitDepth());

    return 1;
}

int Wrap_Decoder::getSampleRate(lua_State* L)
{
    auto* self = luax_checkdecoder(L, 1);

    lua_pushinteger(L, self->getSampleRate());

    return 1;
}

int Wrap_Decoder::getDuration(lua_State* L)
{
    auto* self = luax_checkdecoder(L, 1);

    lua_pushnumber(L, self->getDuration());

    return 1;
}

int Wrap_Decoder::decode(lua_State* L)
{
    auto* self = luax_checkdecoder(L, 1);

    int decoded = self->decode();

    if (decoded > 0)
    {
        luax_catchexcept(L, [&]() {
            auto* soundData = instance()->newSoundData(
                self->getBuffer(), decoded / (self->getBitDepth() / 8 * self->getChannelCount()),
                self->getSampleRate(), self->getBitDepth(), self->getChannelCount());

            luax_pushtype(L, soundData);
            soundData->release();
        });
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Decoder::seek(lua_State* L)
{
    auto* self    = luax_checkdecoder(L, 1);
    double offset = luaL_checknumber(L, 2);

    if (offset < 0)
        return luaL_error(L, "Can't seek to a negative position.");
    else if (offset == 0)
        self->rewind();
    else
        self->seek(offset);

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",            Wrap_Decoder::clone           },
    { "getChannelCount",  Wrap_Decoder::getChannelCount },
    { "getBitDepth",      Wrap_Decoder::getBitDepth     },
    { "getSampleRate",    Wrap_Decoder::getSampleRate   },
    { "getDuration",      Wrap_Decoder::getDuration     },
    { "decode",           Wrap_Decoder::decode          },
    { "seek",             Wrap_Decoder::seek            }
};
// clang-format on

namespace love
{
    Decoder* luax_checkdecoder(lua_State* L, int index)
    {
        return luax_checktype<Decoder>(L, index);
    }

    int open_decoder(lua_State* L)
    {
        return luax_register_type(L, &Decoder::type, functions);
    }
} // namespace love
