#include "modules/thread/wrap_Thread.hpp"

#include "modules/thread/wrap_Channel.hpp"
#include "modules/thread/wrap_LuaThread.hpp"

#include "modules/filesystem/FileData.hpp"
#include "modules/filesystem/physfs/File.hpp"

#include <cstring>

using namespace love;

#define instance() (Module::getInstance<ThreadModule>(Module::M_THREAD))

int Wrap_Thread::newThread(lua_State* L)
{
    std::string name = "Thread code";
    Data* data       = nullptr;

    if (lua_isstring(L, 1))
    {
        size_t length      = 0;
        const char* string = lua_tolstring(L, 1, &length);

        if (length >= 1024 || std::memchr(string, '\n', length))
        {
            lua_pushvalue(L, 1);
            lua_pushstring(L, "string");

            std::array<int, 2> indices = { lua_gettop(L) - 1, lua_gettop(L) };
            luax_convobj(L, indices, "filesystem", "newFileData");

            lua_pop(L, 1);
            lua_replace(L, 1);
        }
        else
            luax_convobj(L, 1, "filesystem", "newFileData");
    }
    else if (luax_istype(L, 1, File::type))
        luax_convobj(L, 1, "filesystem", "newFileData");

    if (luax_istype(L, 1, FileData::type))
    {
        auto* fileData = luax_checktype<FileData>(L, 1);
        name           = std::string("@") + fileData->getFilename();
        data           = fileData;
    }
    else
        data = luax_checktype<Data>(L, 1);

    auto* thread = instance()->newThread(name, data);

    luax_pushtype(L, thread);
    thread->release();

    return 1;
}

int Wrap_Thread::newChannel(lua_State* L)
{
    auto* channel = instance()->newChannel();

    luax_pushtype(L, channel);
    channel->release();

    return 1;
}

int Wrap_Thread::getChannel(lua_State* L)
{
    std::string name = luax_checkstring(L, 1);
    auto* channel    = instance()->getChannel(name);

    luax_pushtype(L, channel);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newThread",  Wrap_Thread::newThread  },
    { "newChannel", Wrap_Thread::newChannel },
    { "getChannel", Wrap_Thread::getChannel }
};

static constexpr lua_CFunction types[] =
{
    love::open_thread,
    love::open_channel
};
// clang-format on

int Wrap_Thread::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new ThreadModule(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "thread";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
