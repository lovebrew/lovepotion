#include "modules/thread/wrap_threadmodule.h"

#include "objects/file/file.h"
#include "objects/filedata/filedata.h"

#include "modules/thread/threadmodule.h"

#include "objects/thread/luathread.h"
#include "objects/thread/wrap_luathread.h"

#include "objects/channel/channel.h"
#include "objects/channel/wrap_channel.h"

#define instance() (Module::GetInstance<ThreadModule>(Module::M_THREAD))

using namespace love;

int Wrap_ThreadModule::NewThread(lua_State* L)
{
    std::string name = "Thread Code";
    love::Data* data = nullptr;

    if (lua_isstring(L, 1))
    {
        size_t length   = 0;
        const char* str = lua_tolstring(L, 1, &length);

        if (length >= 1024 || memchr(str, '\n', length))
        {
            lua_pushvalue(L, 1);
            lua_pushstring(L, "string");

            int indexes[] = { lua_gettop(L) - 1, lua_gettop(L) };
            Luax::ConvertObject(L, indexes, 2, "filesystem", "newFileData");

            lua_pop(L, 1);
            lua_replace(L, 1);
        }
        else
            Luax::ConvertObject(L, 1, "filesystem", "newFileData");
    }
    else if (Luax::IsType(L, 1, File::type))
        Luax::ConvertObject(L, 1, "filesystem", "newFileData");

    if (Luax::IsType(L, 1, FileData::type))
    {
        FileData* fileData = Luax::CheckType<FileData>(L, 1);

        name = std::string("@") + fileData->GetName();
        data = fileData;
    }
    else
        data = Luax::CheckType<Data>(L, 1);

    LuaThread* thread = instance()->NewThread(name, data);
    Luax::PushType(L, thread);

    thread->Release();

    return 1;
}

int Wrap_ThreadModule::NewChannel(lua_State* L)
{
    Channel* channel = instance()->NewChannel();
    Luax::PushType(L, channel);

    channel->Release();

    return 1;
}

int Wrap_ThreadModule::GetChannel(lua_State* L)
{
    std::string name = luaL_checkstring(L, 1);

    Channel* channel = instance()->GetChannel(name);
    Luax::PushType(L, channel);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getChannel", Wrap_ThreadModule::GetChannel },
    { "newChannel", Wrap_ThreadModule::NewChannel },
    { "newThread",  Wrap_ThreadModule::NewThread  },
    { 0,            0                             }
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
    ThreadModule* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new ThreadModule(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "thread";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return Luax::RegisterModule(L, wrappedModule);
}
