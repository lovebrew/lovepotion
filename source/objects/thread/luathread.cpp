#include <common/module.hpp>

#include <modules/event/event.hpp>
#include <modules/event/wrap_event.hpp>
#include <modules/love/love.hpp>

#include <objects/thread/luathread.hpp>

using namespace love;

Type LuaThread::type("Thread", &Threadable::type);

#include <utilities/log/logfile.h>

LuaThread::LuaThread(const std::string& name, Data* code) : code(code), hasError(false)
{
    this->name = name;
}

void LuaThread::ThreadFunction()
{
    this->error.clear();
    this->hasError = false;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    luax::Preload(L, love::Initialize, "love");
    luax::Require(L, "love");
    lua_pop(L, 1);

    luax::Require(L, "love.thread");
    lua_pop(L, 1);

    luax::Require(L, "love.filesystem");
    lua_pop(L, 1);

    lua_pushcfunction(L, luax::Traceback);
    int tracebackIndex = lua_gettop(L);

    if (luaL_loadbuffer(L, (const char*)this->code->GetData(), this->code->GetSize(),
                        this->name.c_str()) != 0)
    {
        this->error    = luax::ToString(L, -1);
        this->hasError = true;
    }
    else
    {
        int pushedCount = (int)this->args.size();

        for (int index = 0; index < pushedCount; index++)
            luax::PushVariant(L, args[index]);

        this->args.clear();

        if (lua_pcall(L, pushedCount, 0, tracebackIndex) != 0)
        {
            this->error    = luax::ToString(L, -1);
            this->hasError = true;
        }
    }

    lua_close(L);

    if (this->hasError)
        this->OnError();
}

bool LuaThread::Start(const std::vector<Variant>& args)
{
    if (this->IsRunning())
        return false;

    this->args = args;

    this->error.clear();
    this->hasError = false;

    return Threadable::Start();
}

const std::string& LuaThread::GetError() const
{
    return this->error;
}

void LuaThread::OnError()
{
    auto event = Module::GetInstance<Event>(Module::M_EVENT);

    if (!event)
        return;

    // clang-format off
    std::vector<Variant> variantArgs = {
        Variant(&LuaThread::type, this),
        Variant(this->error.c_str(), this->error.length())
    };

    StrongReference<Message> message(new Message("threaderror", variantArgs), Acquire::NORETAIN);
    event->Push(message);
}
