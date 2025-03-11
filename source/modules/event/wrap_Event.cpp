#include "common/StrongRef.hpp"

#include "modules/event/Event.hpp"
#include "modules/event/wrap_Event.hpp"

#include <algorithm>

static constexpr char event_lua[] = {
#include "modules/event/wrap_Event.lua"
};

#define E_ARGUMENT_CANNOT_BE_STORED \
    "Argument %d can't be stored safely\nExpected boolean, number, string or userdata."

using namespace love;

#define instance() (Module::getInstance<Event>(Module::M_EVENT))

static int luax_pushmessage(lua_State* L, const Message& message)
{
    luax_pushstring(L, message.name);

    for (const Variant& variant : message.args)
        luax_pushvariant(L, variant);

    return message.args.size() + 1;
}

static int w_poll_i(lua_State* L)
{
    Message* message = nullptr;

    if (instance()->poll(message) && message)
    {
        int args = luax_pushmessage(L, *message);
        message->release();

        return args;
    }

    return 0;
}

int Wrap_Event::pump(lua_State* L)
{
    float timeout = luaL_optnumber(L, 1, 0.0f);
    luax_catchexcept(L, [&] { instance()->pump(timeout); });

    return 0;
}

int Wrap_Event::wait(lua_State* L)
{
    Message* message = nullptr;
    luax_catchexcept(L, [&] { message = instance()->wait(); });

    if (message)
    {
        int args = luax_pushmessage(L, *message);
        message->release();

        return args;
    }

    return 0;
}

int Wrap_Event::push(lua_State* L)
{
    std::string name = luaL_checkstring(L, 1);
    std::vector<Variant> args {};

    const int argc = lua_gettop(L);

    for (int index = 2; index <= argc; index++)
    {
        if (lua_isnoneornil(L, index))
            break;

        luax_catchexcept(L, [&] { args.push_back(luax_checkvariant(L, index)); });

        if (args.back().getType() == Variant::UNKNOWN)
        {
            args.clear();
            return luaL_error(L, E_ARGUMENT_CANNOT_BE_STORED, index);
        }
    }

    StrongRef<Message> message(new Message(name, args), Acquire::NO_RETAIN);
    instance()->push(message);

    luax_pushboolean(L, true);

    return 1;
}

int Wrap_Event::clear(lua_State* L)
{
    luax_catchexcept(L, [&] { instance()->clear(); });

    return 0;
}

int Wrap_Event::quit(lua_State* L)
{

    luax_catchexcept(L, [&] {
        std::vector<Variant> args {};
        const int length = std::max(lua_gettop(L), 1);

        for (int index = 1; index <= length; index++)
            args.push_back(luax_checkvariant(L, index));

        StrongRef<Message> message(new Message("quit", args), Acquire::NO_RETAIN);
        instance()->push(message);
    });

    luax_pushboolean(L, true);

    return 1;
}

int Wrap_Event::restart(lua_State* L)
{

    luax_catchexcept(L, [&] {
        std::vector<Variant> args {};
        const int length = lua_gettop(L);

        args.emplace_back("restart", strlen("restart"));

        for (int index = 1; index <= length; index++)
            args.push_back(luax_checkvariant(L, index));

        StrongRef<Message> message(new Message("quit", args), Acquire::NO_RETAIN);
        instance()->push(message);
    });

    luax_pushboolean(L, true);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clear",   Wrap_Event::clear   },
    { "poll_i",  w_poll_i            },
    { "pump",    Wrap_Event::pump    },
    { "push",    Wrap_Event::push    },
    { "quit",    Wrap_Event::quit    },
    { "restart", Wrap_Event::restart },
    { "wait",    Wrap_Event::wait    }
};
// clang-format on

int Wrap_Event::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Event(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "event";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = {};

    int result = luax_register_module(L, module);

    if (luaL_loadbuffer(L, event_lua, sizeof(event_lua), "event.lua") == 0)
        lua_call(L, 0, 0);
    else
        lua_error(L);

    return result;
}
