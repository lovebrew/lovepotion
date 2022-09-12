#pragma once

#include <common/data.hpp>
#include <common/module.hpp>
#include <common/strongreference.hpp>

#include <objects/channel/channel.hpp>
#include <objects/thread/luathread.hpp>

#include <map>

namespace love
{
    class ThreadModule : public Module
    {
      public:
        virtual ~ThreadModule()
        {}

        virtual const char* GetName() const override
        {
            return "love.thread";
        }

        virtual ModuleType GetModuleType() const override
        {
            return M_THREAD;
        }

        LuaThread* NewThread(const std::string& name, Data* data) const;

        Channel* NewChannel() const;

        Channel* GetChannel(const std::string& name);

      protected:
        std::map<std::string, StrongReference<Channel>> namedChannels;
        love::mutex mutex;
    };
} // namespace love
