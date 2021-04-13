#pragma once

#include "modules/thread/types/mutex.h"
#include "objects/channel/channel.h"
#include "objects/thread/luathread.h"

#include "common/data.h"
#include "common/module.h"
namespace love
{
    class ThreadModule : public Module
    {
      public:
        /* Module implementors */
        ~ThreadModule() {};

        const char* GetName() const override
        {
            return "love.thread";
        }

        ModuleType GetModuleType() const
        {
            return M_THREAD;
        }

        /* Löve2D Functions */

        Channel* NewChannel();

        Channel* GetChannel(const std::string& name);

        LuaThread* NewThread(const std::string& name, love::Data* data);

      private:
        std::map<std::string, StrongReference<Channel>> namedChannels;
        thread::MutexRef namedChannelMutex;
    };
} // namespace love