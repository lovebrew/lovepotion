#include <common/strongreference.hpp>

#include <modules/event/event.hpp>
#include <modules/event/wrap_event.hpp>

#include <modules/love/love.hpp>

static constexpr char wrap_event_lua[] = {
#include <scripts/wrap_event.lua>
};

using namespace love;

#define instance() (Module::GetInstance<Event>(Module::M_EVENT))

static int pushMessage(lua_State* L, const Message& message)
{
    luax::PushString(L, message.name);

    for (auto& variant : message.args)
        luax::PushVariant(L, variant);

    return (int)message.args.size() + 1;
}

static int poll_i(lua_State* L)
{
    Message* message = nullptr;

    if (instance()->Poll(message) && message != nullptr)
    {
        int args = pushMessage(L, *message);
        message->Release();

        return args;
    }

    return 0;
}

int Wrap_Event::Clear(lua_State* L)
{
    luax::CatchException(L, [&]() { instance()->Clear(); });

    return 0;
}

int Wrap_Event::Pump(lua_State* L)
{
    luax::CatchException(L, [&]() { instance()->Pump(); });

    return 0;
}

int Wrap_Event::Push(lua_State* L)
{
    std::string name = luax::CheckString(L, 1);
    std::vector<Variant> args;

    int argCount = lua_gettop(L);

    for (int index = 2; index <= argCount; index++)
    {
        if (lua_isnoneornil(L, index))
            break;

        luax::CatchException(L, [&]() { args.push_back(luax::CheckVariant(L, index)); });

        if (args.back().GetType() == Variant::UNKNOWN)
        {
            args.clear();
            return luaL_error(L,
                              "Argument %d can't be stored safely! Expected boolean, number, "
                              "string or userdata.",
                              index);
        }
    }

    StrongReference<Message> message(new Message(name, args), Acquire::NORETAIN);

    instance()->Push(message);
    luax::PushBoolean(L, true);

    return 1;
}

int Wrap_Event::Wait(lua_State* L)
{
    Message* message = nullptr;

    luax::CatchException(L, [&]() { message = instance()->Wait(); });

    if (message)
    {
        int args = pushMessage(L, *message);
        message->Release();

        return args;
    }

    return 0;
}

int Wrap_Event::Quit(lua_State* L)
{
    luax::CatchException(L, [&]() {
        std::vector<Variant> args;
        for (int index = 1; index <= std::max(1, lua_gettop(L)); index++)
            args.push_back(luax::CheckVariant(L, index));

        StrongReference<Message> message(new Message("quit", args), Acquire::NORETAIN);
        instance()->Push(message);
    });

    lua_pushboolean(L, true);

    return 1;
}

int Wrap_Event::Restart(lua_State* L)
{
    luax::CatchException(L, [&]() {
        std::vector<Variant> args;
        args.emplace_back("restart", strlen("restart"));

        for (int index = 1; index <= lua_gettop(L); index++)
            args.push_back(luax::CheckVariant(L, index));

        StrongReference<Message> message(new Message("quit", args), Acquire::NORETAIN);
        instance()->Push(message);
    });

    luax::PushBoolean(L, true);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clear",     Wrap_Event::Clear   },
    { "poll_i",    poll_i              },
    { "pump",      Wrap_Event::Pump    },
    { "push",      Wrap_Event::Push    },
    { "quit",      Wrap_Event::Quit    },
    { "restart",   Wrap_Event::Restart },
    { "wait",      Wrap_Event::Wait    }
};

// clang-format on

int Wrap_Event::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Event(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "event";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = nullptr;

    int ret = luax::RegisterModule(L, wrappedModule);

    if (luaL_loadbuffer(L, wrap_event_lua, sizeof(wrap_event_lua), "wrap_event.lua") == 0)
        lua_call(L, 0, 0);
    else
        lua_error(L);

    return ret;
}
