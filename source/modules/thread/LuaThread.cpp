#include "modules/thread/LuaThread.hpp"
#include "modules/event/Event.hpp"

#include "common/luax.hpp"

#include "modules/love/love.hpp"

namespace love
{
    Type LuaThread::type("Thread", &Threadable::type);

    LuaThread::LuaThread(const std::string& name, Data* code) :
        code(code),
        name(name),
        _hasError(false)
    {
        this->threadName = name;
    }

    void LuaThread::run()
    {
        this->error.clear();
        this->_hasError = false;

        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        luaopen_bit(L);

        luax_preload(L, love_initialize, "love");
        luax_require(L, "love");
        lua_pop(L, 1);

        luax_require(L, "love.thread");
        lua_pop(L, 1);

        luax_require(L, "love.filesystem");
        lua_pop(L, 1);

        lua_pushcfunction(L, luax_traceback);
        int tracebackIndex = lua_gettop(L);

        // clang-format off
        if (luaL_loadbuffer(L, (const char*)this->code->getData(), this->code->getSize(), this->name.c_str()) != 0)
        {
            this->error = luax_tostring(L, -1);
            this->_hasError = true;
        }
        else
        {
            int pushedArgs = (int)this->args.size();

            for (int index = 0; index < pushedArgs; index++)
                luax_pushvariant(L, this->args[index]);

            this->args.clear();

            if (lua_pcall(L, pushedArgs, 0, tracebackIndex) != 0)
            {
                this->error = luax_tostring(L, -1);
                this->_hasError = true;
            }
        }
        // clang-format on

        lua_close(L);

        if (this->_hasError)
            this->onError();
    }

    bool LuaThread::start(const std::vector<Variant>& args)
    {
        if (isRunning())
            return false;

        this->args = args;
        this->error.clear();
        this->_hasError = false;

        return Threadable::start();
    }

    const std::string& LuaThread::getError() const
    {
        return this->error;
    }

    void LuaThread::onError()
    {
        auto* eventModule = Module::getInstance<Event>(Module::M_EVENT);

        if (!eventModule)
            return;

        // clang-format off
        std::vector<Variant> vargs = {
            Variant(&LuaThread::type, this),
            Variant(this->error.c_str(), this->error.length())
        };
        // clang-format on

        StrongRef<Message> message(new Message("threaderror", vargs), Acquire::NO_RETAIN);
        eventModule->push(message);
    }
} // namespace love
