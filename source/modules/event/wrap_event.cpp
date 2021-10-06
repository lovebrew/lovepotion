#include "modules/event/wrap_event.h"

#include "wrap_event_lua.h"

using namespace love;

#define instance() (Module::GetInstance<love::Event>(Module::M_EVENT))

static int Poll_I(lua_State* L)
{
    Message* message = nullptr;

    if (instance()->Poll(message))
    {
        int args = message->ToLua(L);
        message->Release();

        return args;
    }

    return 0;
}

int Wrap_Event::Clear(lua_State* L)
{
    Luax::CatchException(L, [&]() { instance()->Clear(); });

    return 0;
}

int Wrap_Event::Pump(lua_State* L)
{
    Luax::CatchException(L, [&]() { instance()->Pump(); });

    return 0;
}

int Wrap_Event::Push(lua_State* L)
{
    StrongReference<Message> message;

    Luax::CatchException(L, [&]() { message.Set(Message::FromLua(L, 1), Acquire::NORETAIN); });

    lua_pushboolean(L, message.Get() != nullptr);

    if (message.Get() == nullptr)
        return 1;

    instance()->Push(message);

    return 1;
}

int Wrap_Event::Wait(lua_State* L)
{
    Message* message = nullptr;

    Luax::CatchException(L, [&]() { message = instance()->Wait(); });

    if (message)
    {
        int args = message->ToLua(L);
        message->Release();

        return args;
    }

    return 0;
}

int Wrap_Event::Quit(lua_State* L)
{
    Luax::CatchException(L, [&]() {
        std::vector<Variant> args = { Variant::FromLua(L, 1) };

        StrongReference<Message> message(new Message("quit", args), Acquire::NORETAIN);

        instance()->Push(message);
    });

    lua_pushboolean(L, true);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clear",  Wrap_Event::Clear  },
    { "poll_i", Poll_I             },
    { "pump",   Wrap_Event::Pump   },
    { "push",   Wrap_Event::Push   },
    { "quit",   Wrap_Event::Quit   },
    { "wait",   Wrap_Event::Wait   },
    { 0,        0                  }
};

// clang-format on

int Wrap_Event::Register(lua_State* L)
{
    love::Event* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new love::Event(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "event";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = nullptr;

    int ret = Luax::RegisterModule(L, wrappedModule);

    if (luaL_loadbuffer(L, (const char*)wrap_event_lua, wrap_event_lua_size, "wrap_event.lua") == 0)
        lua_call(L, 0, 0);
    else
        lua_error(L);

    return ret;
}
