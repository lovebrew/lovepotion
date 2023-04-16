#include "objects/thread/luathread.h"

#include "modules/event/event.h"
#include "modules/love.h"

using namespace love;

love::Type LuaThread::type("Thread", &Threadable::type);

LuaThread::LuaThread(const std::string& name, love::Data* code) : code(code), name(name)
{
    this->threadName = name;
}

LuaThread::~LuaThread()
{}

void LuaThread::ThreadFunction()
{
    this->error.clear();
    this->hasError = false;

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    Luax::Preload(L, love::Initialize, "love");
    Luax::Require(L, "love");
    lua_pop(L, 1);

    Luax::Require(L, "love.thread");
    lua_pop(L, 1);

    Luax::Require(L, "love.filesystem");
    lua_pop(L, 1);

    lua_pushcfunction(L, Luax::Traceback);
    int tracebackIndex = lua_gettop(L);

    if (luaL_loadbuffer(L, (const char*)this->code->GetData(), this->code->GetSize(),
                        this->name.c_str()) != 0)
    {
        this->error    = lua_tostring(L, -1);
        this->hasError = true;
    }
    else
    {
        int pushedArgs = (int)this->args.size();

        for (int i = 0; i < pushedArgs; i++)
            this->args[i].ToLua(L);

        this->args.clear();

        if (lua_pcall(L, pushedArgs, 0, tracebackIndex) != 0)
            this->error = lua_tostring(L, -1);
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
    auto eventModule = Module::GetInstance<love::Event>(Module::M_EVENT);

    if (!eventModule)
        return;

    std::vector<Variant> vargs = { Variant(&LuaThread::type, this),
                                   Variant(this->error.c_str(), this->error.length()) };

    StrongReference<Message> message(new Message("threaderror", vargs), Acquire::NORETAIN);
    eventModule->Push(message);
}
