#include "common/luax.h"
#include "modules/thread/wrap_threadmodule.h"

#include "objects/file/file.h"
#include "objects/filedata/filedata.h"

#define instance() (Module::GetInstance<ThreadModule>(Module::M_THREAD))

using namespace love;

int Wrap_ThreadModule::NewThread(lua_State * L)
{
    std::string name = "Thread Code";
    love::Data * data = nullptr;

    if (lua_isstring(L, 1))
    {
        size_t length = 0;
        const char * str = lua_tolstring(L, 1, &length);

        if (length >= 1024 || memchr(str, '\n', length))
        {
            lua_pushvalue(L, 1);
            lua_pushstring(L, "string");

            int indexes[] = {lua_gettop(L) - 1, lua_gettop(L)};
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
        FileData * fileData = Luax::CheckType<FileData>(L, 1);

        name = std::string("@") + fileData->GetName();
        data = fileData;
    }
    else
        data = Luax::CheckType<Data>(L, 1);

    LuaThread * thread = instance()->NewThread(name, data);
    Luax::PushType(L, thread);

    thread->Release();

    return 1;
}

int Wrap_ThreadModule::NewChannel(lua_State * L)
{
    Channel * channel = instance()->NewChannel();
    Luax::PushType(L, channel);

    channel->Release();

    return 1;
}

int Wrap_ThreadModule::GetChannel(lua_State * L)
{
    std::string name = luaL_checkstring(L, 1);

    Channel * channel = instance()->GetChannel(name);
    Luax::PushType(L, channel);

    return 1;
}

int Wrap_ThreadModule::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "newThread",  NewThread  },
        { "newChannel", NewChannel },
        { "getChannel", GetChannel },
        { 0,            0          }
    };

    lua_CFunction types[] =
    {
        Wrap_LuaThread::Register,
        Wrap_Channel::Register,
        0
    };

    ThreadModule * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() {instance = new ThreadModule();});
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance = instance;
    wrappedModule.name = "thread";
    wrappedModule.type = &Module::type;
    wrappedModule.functions = reg;
    wrappedModule.types = types;

    return Luax::RegisterModule(L, wrappedModule);
}