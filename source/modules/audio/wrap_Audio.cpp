#include "modules/audio/wrap_Audio.hpp"
#include "modules/audio/wrap_Source.hpp"

#include "modules/filesystem/wrap_Filesystem.hpp"

using namespace love;

#define instance() Module::getInstance<Audio>(Module::M_AUDIO)

int Wrap_Audio::newSource(lua_State* L)
{
    auto type = Source::TYPE_STREAM;

    if (!luax_istype(L, 1, Decoder::type))
    {
        if (!luax_istype(L, 1, SoundData::type))
        {
            const char* typeName = luaL_checkstring(L, 2);
            if (typeName && !Source::getConstant(typeName, type))
                return luax_enumerror(L, "source type", Source::SourceTypes, typeName);

            if (type == Source::TYPE_QUEUE)
                return luaL_error(L, E_CANNOT_CREATE_QUEUE_SOURCE);
        }

        if (luax_cangetdata(L, 1))
        {
            if (type == Source::TYPE_STATIC)
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

    if (type == Source::TYPE_STATIC && luax_istype(L, 1, Decoder::type))
        luax_convobj(L, 1, "sound", "newSoundData");

    Source* source = nullptr;

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

int Wrap_Audio::play(lua_State* L)
{
    if (lua_istable(L, 1))
        return 0;
    else if (lua_gettop(L) > 1)
        return 0;
    else
    {
        auto* source = luax_checksource(L, 1);
        luax_pushboolean(L, source->play());

        return 1;
    }
}

int Wrap_Audio::stop(lua_State* L)
{
    if (lua_isnone(L, 1))
        return 0;
    if (lua_istable(L, 1))
        return 0;
    else if (lua_gettop(L) > 1)
        return 0;
    else
    {
        auto* source = luax_checksource(L, 1);
        source->stop();
    }

    return 0;
}

int Wrap_Audio::pause(lua_State* L)
{
    if (lua_istable(L, 1))
        return 0;
    else if (lua_gettop(L) > 1)
        return 0;
    else
    {
        auto* source = luax_checksource(L, 1);
        source->pause();
    }

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[]=
{
    { "newSource", Wrap_Audio::newSource },
    { "play",      Wrap_Audio::play      },
    { "stop",      Wrap_Audio::stop      },
    { "pause",     Wrap_Audio::pause     }
};

static constexpr lua_CFunction types[]=
{
    love::open_source
};
// clang-format on

int Wrap_Audio::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&] { instance = new Audio(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "audio";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
