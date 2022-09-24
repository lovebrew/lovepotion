#include <modules/audio/audio.hpp>
#include <modules/audio/wrap_audio.hpp>

#include <objects/source/wrap_source.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>

using namespace love;
using Source = love::Source<Console::Which>;

#define instance() (Module::GetInstance<Audio>(Module::M_AUDIO))

int Wrap_Audio::GetActiveSourceCount(lua_State* L)
{
    lua_pushinteger(L, instance()->GetActiveSourceCount());

    return 1;
}

int Wrap_Audio::NewSource(lua_State* L)
{
    auto type = ::Source::TYPE_STREAM;
    if (!luax::IsType(L, 1, SoundData::type))
    {
        if (!luax::IsType(L, 1, Decoder::type))
        {
            auto* typeName = luaL_checkstring(L, 2);
            if (typeName && !::Source::GetConstant(typeName, type))
                return luax::EnumError(L, "source type", ::Source::GetConstants(type), typeName);

            if (type == ::Source::TYPE_QUEUE)
                return luaL_error(L, "Cannot create queueable sources using newSource. Use "
                                     "newQueueableSource instead.");
        }

        if (Wrap_Filesystem::CanGetData(L, 1))
        {
            if (type == ::Source::TYPE_STATIC)
                lua_pushstring(L, "memory");
            else if (!lua_isnone(L, 3))
                lua_pushvalue(L, 3);
            else
                lua_pushnil(L);

            lua_pushnil(L);

            int indexes[] = { 1, lua_gettop(L), lua_gettop(L) - 1 };
            luax::ConvertObject(L, indexes, 3, "sound", "newDecoder");
        }
    }

    if (type == ::Source::TYPE_STATIC && luax::IsType(L, 1, Decoder::type))
        luax::ConvertObject(L, 1, "sound", "newSoundData");

    ::Source* self = nullptr;

    luax::CatchException(L, [&]() {
        if (luax::IsType(L, 1, SoundData::type))
            self = instance()->NewSource(luax::ToType<SoundData>(L, 1));
        else if (luax::IsType(L, 1, Decoder::type))
            self = instance()->NewSource(luax::ToType<Decoder>(L, 1));
    });

    if (!self)
        return luax::TypeError(L, 1, "Decoder or SoundData");

    luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_Audio::NewQueueableSource(lua_State* L)
{
    return 0;
}

static std::vector<::Source*> readSourceList(lua_State* L, int stackPosition)
{
    if (stackPosition < 0)
        stackPosition += lua_gettop(L) + 1;

    int count = luax::ObjectLength(L, stackPosition);
    std::vector<::Source*> sources(count);

    for (int index = 0; index < count; index++)
    {
        lua_rawgeti(L, stackPosition, index + 1);
        sources[index] = Wrap_Source::CheckSource(L, -1);
        lua_pop(L, 1);
    }

    return sources;
}

static std::vector<::Source*> readSourceVararg(lua_State* L, int stackPosition)
{
    const int top = lua_gettop(L);

    if (stackPosition < 0)
        stackPosition += top + 1;

    int count = (top - stackPosition + 1);
    std::vector<::Source*> sources(count);

    for (int position = 0; position <= top; stackPosition++, position++)
        sources[position] = Wrap_Source::CheckSource(L, stackPosition);

    return sources;
}

int Wrap_Audio::Play(lua_State* L)
{
    if (lua_istable(L, 1))
        return 0;
    else if (lua_gettop(L) > 1)
        return 0;
    else
    {
        auto* self = Wrap_Source::CheckSource(L, 1);
        luax::PushBoolean(L, instance()->Play(self));
    }

    return 1;
}

/* todo */
int Wrap_Audio::Stop(lua_State* L)
{}

/* todo */
int Wrap_Audio::Pause(lua_State* L)
{}

/* todo */
int Wrap_Audio::SetVolume(lua_State* L)
{}

/* todo */
int Wrap_Audio::GetVolume(lua_State* L)
{}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getActiveSourceCount", Wrap_Audio::GetActiveSourceCount },
    { "newSource",            Wrap_Audio::NewSource            },
    { "play",                 Wrap_Audio::Play                 }
};

static constexpr lua_CFunction types[] =
{
    nullptr
};
// clang-format on

int Wrap_Audio::Register(lua_State* L)
{
    auto* instance = instance();

    if (!instance)
        luax::CatchException(L, [&]() { instance = new Audio(); });
    else
        instance->Retain();

    WrappedModule wrappedModule {};
    wrappedModule.instance  = instance;
    wrappedModule.name      = "audio";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
