#include "common/runtime.h"
#include "modules/event/wrap_event.h"

#include "wrap_Event_lua.h"

using namespace love;

#define instance() (Module::GetInstance<love::Event>(Module::M_EVENT))

int Wrap_Event::Poll_I(lua_State * L)
{
    Message * message = nullptr;

    if (instance()->Poll(message))
    {
        int args = message->ToLua(L);
        delete message;

        return args;
    }

    return 0;
}

int Wrap_Event::Clear(lua_State * L)
{
    Luax::CatchException(L, [&]() {
        instance()->Clear();
    });

    return 0;
}

int Wrap_Event::Pump(lua_State * L)
{
    Luax::CatchException(L, [&]() {
        instance()->Pump();
    });

    return 0;
}

int Wrap_Event::Push(lua_State * L)
{
    StrongReference<Message> message;

    Luax::CatchException(L, [&]() {
        message.Set(Message::FromLua(L, 1), Acquire::NORETAIN);
    });

    lua_pushboolean(L, message.Get() != nullptr);

    if (message.Get() != nullptr)
        return 1;

    instance()->Push(message);

    return 1;
}

int Wrap_Event::Wait(lua_State * L)
{
    /* TO DO */

    return 0;
}

int Wrap_Event::Quit(lua_State * L)
{
    Luax::CatchException(L, [&]() {
        std::vector<Variant> args = {Variant::FromLua(L, 1)};

        StrongReference<Message> message(new Message("quit", args), Acquire::NORETAIN);

        instance()->Push(message);
    });

    lua_pushboolean(L, true);

    return 1;
}

int Wrap_Event::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "poll_i", Poll_I },
        { "clear",  Clear  },
        { "pump",   Pump   },
        { "push",   Push   },
        { "quit",   Quit   },
        { 0,        0      }
    };

    love::Event * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new love::Event(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "event";
    module.functions = reg;
    module.type = &Module::type;
    module.types = 0;

    int ret = Luax::RegisterModule(L, module);

    if (luaL_loadbuffer(L, (const char *)wrap_event_lua, wrap_event_lua_size, "wrap_event.lua") == 0)
        lua_call(L, 0, 0);
    else
        lua_error(L);

    return ret;
}
