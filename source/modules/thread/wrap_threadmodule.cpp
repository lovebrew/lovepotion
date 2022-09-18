#include <modules/thread/threadmodule.hpp>
#include <modules/thread/wrap_threadmodule.hpp>

#include <objects/channel/wrap_channel.hpp>
#include <objects/thread/wrap_luathread.hpp>

#include <common/data.hpp>

#include <objects/file/file.hpp>
#include <objects/thread/luathread.hpp>

#include <string.h>

using namespace love;

#define instance() (Module::GetInstance<ThreadModule>(Module::M_THREAD))

int Wrap_ThreadModule::NewThread(lua_State* L)
{
    std::string name = "Thread Code";
    Data* data       = nullptr;

    if (lua_isstring(L, 1))
    {
        size_t stringLength = 0;
        const char* string  = lua_tolstring(L, 1, &stringLength);

        if (stringLength >= 0x400 || memchr(string, '\n', stringLength))
        {
            lua_pushvalue(L, 1);
            lua_pushstring(L, "string");

            int indexes[] = { lua_gettop(L) - 1, lua_gettop(L) };
            luax::ConvertObject(L, indexes, 2, "filesystem", "newFileData");

            lua_pop(L, 1);
            lua_replace(L, 1);
        }
        else
            luax::ConvertObject(L, 1, "filesystem", "newFileData");
    }
    else if (luax::IsType(L, 1, File::type))
        luax::ConvertObject(L, 1, "filesystem", "newFileData");

    if (luax::IsType(L, 1, FileData::type))
    {
        FileData* fileData = luax::CheckType<FileData>(L, 1);
        name               = "@" + fileData->GetFilename();
        data               = fileData;
    }
    else
        data = luax::CheckType<Data>(L, 1);

    LuaThread* self = instance()->NewThread(name, data);

    luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_ThreadModule::NewChannel(lua_State* L)
{
    auto* channel = instance()->NewChannel();

    luax::PushType(L, channel);
    channel->Release();

    return 1;
}

int Wrap_ThreadModule::GetChannel(lua_State* L)
{
    std::string name = luax::CheckString(L, 1);
    auto* channel    = instance()->GetChannel(name);

    luax::PushType(L, channel);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getChannel", Wrap_ThreadModule::GetChannel },
    { "newChannel", Wrap_ThreadModule::NewChannel },
    { "newThread",  Wrap_ThreadModule::NewThread  }
};

static constexpr lua_CFunction types[] =
{
    Wrap_LuaThread::Register,
    Wrap_Channel::Register,
    nullptr
};
// clang-format on

int Wrap_ThreadModule::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new ThreadModule(); });
    else
        instance->Retain();

    WrappedModule wrappedModule {};

    wrappedModule.instance  = instance;
    wrappedModule.name      = "thread";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
